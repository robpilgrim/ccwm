#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#define MAX(a,b) (a > b ? a : b)

Display *d;
Window root;
XWindowAttributes xattr;
XEvent ev;
XKeyEvent kev;
XButtonEvent bev;
KeySym keysym;
int true = 1;

char* menu[] = {"dmenu_run", NULL};

void exec(Display *d, char* arg[]) {
    if(fork()==0) {
        if(d) close(ConnectionNumber(d));
        setsid();

        int status = execvp(arg[0], arg);
        if(status < 1) __builtin_printf("Error");
    }
}

void keypress(Display *d, KeySym keysym, Window sw) {

   switch(keysym) {
       case XK_q:
           if(ev.xkey.state & ((Mod4Mask | ShiftMask) == (Mod4Mask | ShiftMask))) {
               true=0;
           }
           else {
               if(ev.xkey.subwindow){
                    ev.xclient.type = ClientMessage;
                    ev.xclient.window = sw;
                    ev.xclient.format = 32;
                    ev.xclient.data.l[0] = XInternAtom(d, "WM_DELETE_WINDOW",0);
                    XSendEvent(d, sw, False, NoEventMask, &ev);
               }
           }
           break;

       case XK_d:
               exec(d, menu);
               break;

       default: break;
   } 
}

int main(void) {

    if(!(d = XOpenDisplay(0x0))) return 1;

    root = DefaultRootWindow(d);

    XGrabKey(d,XKeysymToKeycode(d,XStringToKeysym("d")),Mod4Mask,root,True,GrabModeAsync,GrabModeAsync);
    XGrabKey(d,XKeysymToKeycode(d,XStringToKeysym("r")),Mod4Mask,root,True,GrabModeAsync,GrabModeAsync);
    XGrabKey(d,XKeysymToKeycode(d,XStringToKeysym("f")),Mod4Mask,root,True,GrabModeAsync,GrabModeAsync);
    XGrabKey(d,XKeysymToKeycode(d,XStringToKeysym("q")),AnyModifier,root,True,GrabModeAsync,GrabModeAsync);

    XGrabButton(d, 1, Mod4Mask, root, True, ButtonPressMask, GrabModeAsync,GrabModeAsync, None, None);
		XGrabButton(d, 3, Mod4Mask, root, True, ButtonPressMask, GrabModeAsync,GrabModeAsync, None, None);

    int xdiff, ydiff;
    while(true) {
        XNextEvent(d, &ev);

        if(ev.type == ButtonPress && ev.xbutton.subwindow != None)
            XRaiseWindow(d, ev.xbutton.subwindow);

        switch(ev.type) {
            case KeyPress:
                kev = ev.xkey;
                keysym  = XkbKeycodeToKeysym(d, kev.keycode,0,0);
                keypress(d, keysym,ev.xkey.subwindow);
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
								break;           break;
            case KeyRelease:
                break;
            default: __builtin_printf("1");
        }
    }
}
