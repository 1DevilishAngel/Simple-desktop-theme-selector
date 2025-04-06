#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <glib.h>
#include <iostream>
#include <fstream>
#include <gtk/gtk.h>
#include <ostream>
#include <string>
#include <unistd.h>
#include <pwd.h>


static GtkWidget *window;

static void RestoreDefaults() {
  std::cout << "restoring defaults...";
  std::string BaseDirectory = (std::string)getpwuid(getuid())->pw_dir + "/.local/share/SDTS";
  for (const auto& ThemeEntry : std::filesystem::directory_iterator(BaseDirectory)){
      std::string LinksDirectory = ThemeEntry.path().string() + "/Directories";
      for (const auto& DirectoryEntry : std::filesystem::recursive_directory_iterator(LinksDirectory)) {
        std::string LinkPath = (std::string)getpwuid(getuid())->pw_dir + DirectoryEntry.path().string().substr(LinksDirectory.length(),LinkPath.length() - LinksDirectory.length());
        if (std::filesystem::exists(LinkPath) && std::filesystem::is_directory(LinkPath)) {
          for (auto const& dir_entry : std::filesystem::directory_iterator(LinkPath, std::filesystem::directory_options::skip_permission_denied)) {
            if (dir_entry.path().string().substr(dir_entry.path().string().length() -7, 7) == "SDTSDEF") {
              std::string ReplacementPath = dir_entry.path().string().substr(0,dir_entry.path().string().length() -8);
              if (std::filesystem::exists(ReplacementPath)) {
                std::filesystem::remove(ReplacementPath);
            }
            std::cout << ReplacementPath;
            std::filesystem::rename(dir_entry,ReplacementPath);
           }
        }
      }
    }
  }
}


static void RunConfig (GtkWidget *Widget) {
  std::string BaseDirectory = (std::string)getpwuid(getuid())->pw_dir + "/.local/share/SDTS";
  std::string ThemeDirectory = BaseDirectory + "/" + gtk_widget_get_name(Widget);
  std::string LinksDirectory = ThemeDirectory + "/Directories";
  RestoreDefaults();
  for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(LinksDirectory)) {
    std::cout << dir_entry << '\n';
    if (dir_entry.is_symlink()) {
      std::string LinkPath = (std::string)getpwuid(getuid())->pw_dir + dir_entry.path().string().substr(LinksDirectory.length(),LinkPath.length() - LinksDirectory.length());
      std::cout << LinkPath << std::endl;
      if (std::filesystem::exists(LinkPath)) {
        std::filesystem::rename(LinkPath,LinkPath + ".SDTSDEF");
      }
      std::filesystem::create_directory_symlink(dir_entry,LinkPath);
    }
  }
  //std::cout << ThemeDirectory;
  std::string Command = "sh " + ThemeDirectory + "/Init.sh";
  system(Command.c_str());
  gtk_window_destroy(GTK_WINDOW(window));
}

static void activate (GtkApplication* app, gpointer user_data)
{
  std::string name;
  GtkWidget *base;
  GtkWidget *button;
  GtkWidget *ButtonIcon;
  GtkWidget *ButtonContainer;
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
  gtk_window_present (GTK_WINDOW (window));
  base = gtk_grid_new();
  gtk_window_set_child (GTK_WINDOW (window), base); 
  int I = 0;
  button = gtk_button_new_with_label("Restore Default");
  g_signal_connect (button, "clicked", G_CALLBACK (RestoreDefaults), NULL);
  g_signal_connect(button, "clicked", G_CALLBACK (gtk_window_destroy), window);
  gtk_grid_attach (GTK_GRID (base), button, 0, 0, 2, 1);
  I++;
  std::string BaseDirectory = (std::string)getpwuid(getuid())->pw_dir + "/.local/share/SDTS";
  for (const auto& entry : std::filesystem::directory_iterator(BaseDirectory)) {
    const auto filenameStr = entry.path().filename().string();
    std::string IconPath = BaseDirectory + "/" + filenameStr + "/Icon.png";
    name = filenameStr;
    button = gtk_button_new_with_label(name.c_str());
    if (std::filesystem::exists(IconPath)) {
      ButtonIcon = gtk_image_new_from_file(IconPath.c_str());
      ButtonContainer = gtk_grid_new();
      gtk_grid_attach(GTK_GRID(ButtonContainer), ButtonIcon, 0, 0, 4, 4);
      gtk_grid_attach(GTK_GRID(ButtonContainer), gtk_label_new(name.c_str()), 12, 0, 2, 2);
      gtk_button_set_child(GTK_BUTTON(button), ButtonContainer);
    }
    gtk_widget_set_name(button, name.c_str());
    //gpointer* pointer = &filenameStr; 
    g_signal_connect (button, "clicked", G_CALLBACK (RunConfig), NULL);
    gtk_grid_attach (GTK_GRID (base), button, 0, I, 2, 1);
    I++;
  }

}

int
main (int argc, char **argv)
{
  GtkApplication *app;
  int status;
  std::string BaseDirectory = (std::string)getpwuid(getuid())->pw_dir + "/.local/share/SDTS";
  if (!std::filesystem::exists(BaseDirectory)) {
      std::filesystem::create_directory(BaseDirectory);
  }
  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}