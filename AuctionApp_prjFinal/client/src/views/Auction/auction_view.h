#ifndef AUCTION_VIEW_H
#define AUCTION_VIEW_H
#include <gtk/gtk.h>
#include "room.h"
#include "item.h"

// Khai báo hàm hoặc cấu trúc
void init_auction_view(int sockfd, GtkWidget *home_window, Room room, Item item, int role);

#endif