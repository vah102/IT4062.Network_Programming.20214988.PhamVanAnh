# Compiler và cờ biên dịch
CC = gcc
CFLAGS = -Wall -rdynamic
GTK_FLAGS = `pkg-config --cflags --libs gtk+-3.0`

# Thư mục chính
CLIENT_DIR = client/src
AUTH_DIR = $(CLIENT_DIR)/views/Auth
UTILS_DIR = $(CLIENT_DIR)/utils
SERVICES_DIR = $(CLIENT_DIR)/services

SERVER_DIR = server/src
MODELS_DIR = $(SERVER_DIR)/models
CONTROLLERS_DIR = $(SERVER_DIR)/controllers

INCLUDE_DIR = include

# Tệp nguồn và đầu ra
CLIENT_SOURCES = 	$(CLIENT_DIR)/main.c \
					$(AUTH_DIR)/auth_view.c \
					$(UTILS_DIR)/style_manager.c \
					$(SERVICES_DIR)/auth_service.c \
					$(MODELS_DIR)/user/user.c \

SERVER_SOURCES = 	$(SERVER_DIR)/main.c \
					$(CONTROLLERS_DIR)/server_controller.c \
					$(MODELS_DIR)/user/user.c \
					$(MODELS_DIR)/room/room.c \
					$(MODELS_DIR)/item/item.c 			

CLIENT = client_exec
SERVER = server_exec

INCLUDES = 	-I$(AUTH_DIR) -I$(UTILS_DIR) -I$(SERVICES_DIR) \
			-I$(MODELS_DIR)/user -I$(CONTROLLERS_DIR) \
			-I$(INCLUDE_DIR) \
			-I$(MODELS_DIR)/room\
			-I$(MODELS_DIR)/item

# Quy tắc biên dịch
all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_SOURCES) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(CLIENT) $(CLIENT_SOURCES) $(GTK_FLAGS)

$(SERVER): $(SERVER_SOURCES) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(SERVER) $(SERVER_SOURCES) $(GTK_FLAGS)

# Quy tắc dọn dẹp
clean:
	rm -f $(CLIENT) $(SERVER)