#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xcb/xcb.h>
/*   
Command to start up zephyr for testing
Xephyr -br -ac -screen 800x600 -reset -terminate 2> /dev/null :2 & DISPLAY=:2 xterm &
*/
xcb_connection_t *connection;
xcb_screen_t *screen;
bool exitFlag = true;
xcb_generic_event_t *e;
uint16_t rootWidth, rootHeight;
uint32_t maskWindow;
int screen_default_nbr;
uint32_t rootMask[1] = { XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
					 XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_BUTTON_PRESS};
uint32_t values[2];

const uint16_t reconWinMask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;

struct workspaceWindow {
  struct workspaceWindow *nextWindow;
  xcb_window_t currWindow;
};

struct workspace {
  struct workspaceWindow *nextWindow;
  int numOfWindows;
};

struct workspace *Workspace;
/* Create a workspace */
void initWorkspace() {
  Workspace = malloc(sizeof(struct workspace));
  // Initialize
  Workspace->nextWindow = NULL;
  Workspace->numOfWindows = 0;
}

void mapRequest(xcb_map_request_event_t *mapEvent) {
  xcb_window_t newWindow = mapEvent->window;
  /* xcb_get_geometry_cookie_t replyCookie = xcb_get_geometry(connection, newWindow); */
  /* xcb_get_geometry_reply_t *geometryReply = xcb_get_geometry_reply(connection, replyCookie, NULL); */
  /* geometryReply-> */
  if (Workspace->numOfWindows == 0) {
    uint32_t reconValues[4] = { 0,0,rootWidth, rootHeight};
	xcb_configure_window(connection, newWindow, reconWinMask, reconValues);
	xcb_flush(connection);
  }
  struct workspaceWindow *window = malloc(sizeof(struct workspaceWindow));
  Workspace->nextWindow = window;
  Workspace->numOfWindows += 1;
}

int main(int argc, char *argv[]) {
  // Get the current screen
  screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
  // Make the root window detect events which modify
  xcb_change_window_attributes(connection, screen->root, XCB_CW_EVENT_MASK, rootMask);

  // Width and height of the current window
  rootWidth = screen->width_in_pixels;
  rootHeight = screen->height_in_pixels;

  // Create a test window to detect events
  /* window = xcb_generate_id(connection); */
  /* maskWindow = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK; */
  /* values[0] = screen->white_pixel; */
  /* values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS | */
  /*             XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION | */
  /*             XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW | */
  /*             XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE; */

  /* xcb_create_window(connection, 0, window, screen->root, 550, 200, 150, 300, 10, */
  /*                   XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, maskWindow, */
  /*                   values); */
  /* xcb_map_window(connection, window); */
  /* xcb_flush(connection); */

  // Main loop
  // Either catch an event or check if the program must shutdown
  while ((e = xcb_wait_for_event(connection)) && exitFlag) {
    switch (e->response_type) {
    case XCB_ENTER_NOTIFY:
      printf("Have to enter notify you\n");
      break;
	case XCB_CREATE_NOTIFY:
	  // First event
	  printf("Create notify\n");
	  break;
	case XCB_MAP_NOTIFY:
	  // Second event
	  printf("Map notify\n");
	  break;
	case XCB_MAPPING_NOTIFY:
	  printf("Mapping notify\n");
	  break;
	case XCB_CONFIGURE_NOTIFY:
	  printf("Configure request\n");
	  break;
    case XCB_MAP_REQUEST:
	  // This even is the first one called when a new window wants to be created
      printf("Map request Event caught\n");
	  xcb_map_request_event_t *mapRequestEvent = (xcb_map_request_event_t *)e;
	  mapRequest(mapRequestEvent);
    case XCB_EXPOSE:
	  // Third event
      printf("Caught an expose event!\n");
      break;
    case XCB_KEY_PRESS: {
	  printf("Key press detected\n");
      break;
    }
    default:
      printf("not handled\n");
    }
    free(e);
  }
  return 0;
}
