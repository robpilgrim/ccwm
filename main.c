#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX(a,b) (a > b ? a : b)
#define TABLENGTH(X) (sizeof(X)/sizeof(*X))

Display *d;
Window root;
XWindowAttributes xattr;
XEvent ev;
XKeyEvent kev;
XButtonEvent bev;
KeySym keysym;
int done = 0;

const char* menu[] = {"dmenu_run", NULL};

void quit() {
    done = 1;
}

typedef union {
    const char** com;
    const int i;
} Arg;

struct key {
    unsigned int mod;
    KeySym keysym;
    void (*function)(const Arg arg);
    const Arg arg;
};

void close_window() {
    ev.xclient.type = ClientMessage;
    ev.xclient.window = ev.xkey.subwindow;
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = XInternAtom(d, "WM_DELETE_WINDOW",0);
    XSendEvent(d, ev.xkey.subwindow, False, NoEventMask, &ev);
}


void spawn(const Arg arg) {
    if(fork() == 0) {
        if(fork() == 0) {
            if(d)
                close(ConnectionNumber(d));

            setsid();
            execvp((char*)arg.com[0],(char**)arg.com);
        }
        exit(0);
    }
}

static struct key keys[] = {
    {Mod4Mask | ShiftMask, XK_q, quit, {NULL}},
    {Mod4Mask, XK_q, close_window, {NULL}},
    {Mod4Mask, XK_d, spawn, {.com = menu}}
};

void grabkeys() {
    long unsigned int i;
    KeyCode code;

    // For each shortcuts
    for(i=0;i<TABLENGTH(keys);++i) {
        if((code = XKeysymToKeycode(d,keys[i].keysym))) {
            XGrabKey(d,code,keys[i].mod,root,True,GrabModeAsync,GrabModeAsync);
        }
    }
}

void keypress(XEvent *e) {
    long unsigned int i;
    XKeyEvent ke = e->xkey;
    KeySym keysym = XkbKeycodeToKeysym(d,ke.keycode,0,0);

    for(i=0;i<TABLENGTH(keys);++i) {
        if(keys[i].keysym == keysym && keys[i].mod == ke.state) {
            keys[i].function(keys[i].arg);
        }
    }
}


int main(void) {

    if(!(d = XOpenDisplay(0x0))) return 1;

    root = DefaultRootWindow(d);
    grabkeys();

    XGrabButton(d, 1, Mod4Mask, root, True, ButtonPressMask, GrabModeAsync,GrabModeAsync, None, None);
		XGrabButton(d, 3, Mod4Mask, root, True, ButtonPressMask, GrabModeAsync,GrabModeAsync, None, None);

    int xdiff, ydiff;
    while(!done) {
        XNextEvent(d, &ev);

        if(ev.type == ButtonPress && ev.xbutton.subwindow != None)
            XRaiseWindow(d, ev.xbutton.subwindow);

        switch(ev.type) {
            case KeyPress:
                kev = ev.xkey;
                keysym  = XkbKeycodeToKeysym(d, kev.keycode,0,0);
                keypress(&ev);
                break;
            case ButtonPress:
                XGrabPointer(d, ev.xbutton.subwindow, True,
                        PointerMotionMask|ButtonReleaseMask, GrabModeAsync,
                        GrabModeAsync, None, None, CurrentTime);

                XGetWindowAttributes(d, ev.xbutton.subwindow, &xattr);
                bev = ev.xbutton;
                break;
            case MotionNotify: 
                while(XCheckTypedEvent(d, MotionNotify, &ev));
                xdiff = ev.xbutton.x_root - bev.x_root;
                ydiff = ev.xbutton.y_root - bev.y_root;
                XMoveResizeWindow(d, ev.xmotion.window,
                        xattr.x + (bev.button==1 ? xdiff : 0),
                        xattr.y + (bev.button==1 ? ydiff : 0),
                        MAX(1, xattr.width + (bev.button==3 ? xdiff : 0)),
                        MAX(1, xattr.height + (bev.button==3 ? ydiff : 0)));
                break;
            case ButtonRelease:
                XUngrabPointer(d,CurrentTime);
                break;
            case KeyRelease:
                break;
            default: printf("ok");
        }
    }
}
