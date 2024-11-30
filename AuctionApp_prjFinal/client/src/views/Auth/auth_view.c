#include <gtk/gtk.h>
#include "style_manager.h"
#include "auth_view.h"
#include "auth_service.h"

typedef struct
{
    int sockfd;
    GtkWidget *entry_username;
    GtkWidget *entry_password;
} LoginContext;

// Hàm đóng ứng dụng khi cửa sổ bị đóng
void on_main_window_destroy()
{
    gtk_main_quit();
}

void on_register_button_clicked(GtkWidget *button, gpointer user_data)
{
    printf("Register button clicked\n");
}

void on_login_button_clicked(GtkWidget *button, gpointer user_data)
{
    LoginContext *context = (LoginContext *)user_data;

    const gchar *username = gtk_entry_get_text(GTK_ENTRY(context->entry_username));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(context->entry_password));

    printf("Username: %s, Password: %s\n", username, password);

    int result = handle_login(context->sockfd, username, password);
    if(result) {
        printf("Đăng nhập thành công\n");
    }
    else printf("Đăng nhập thất bại\n");
}

void init_auth_view(int sockfd)
{
    GtkBuilder *builder;
    GtkWidget *window;
    GError *error = NULL;

    gtk_init(NULL, NULL);

    builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(builder, "client/src/views/Auth/auth_view.glade", &error))
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return;
    }

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_auth"));
    g_signal_connect(window, "destroy", G_CALLBACK(on_main_window_destroy), NULL);

    LoginContext *context = g_malloc(sizeof(LoginContext));
    context->sockfd = sockfd;
    context->entry_username = GTK_WIDGET(gtk_builder_get_object(builder, "login_username"));
    context->entry_password = GTK_WIDGET(gtk_builder_get_object(builder, "login_password"));

    gtk_builder_connect_signals(builder, context);

    apply_css();

    gtk_widget_show_all(window);
    gtk_main();

    g_object_unref(builder);
    g_free(context);
}
