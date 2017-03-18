#include <QtGui/QApplication>

#include <gui/mokawindow.h>

/**
 * Function main
 *
 * Starts the application showing the main window.
 */
int main(int argc, char *argv[])
{
  QApplication qapp(argc, argv);

  // Application info (required to use QSettings)
  QCoreApplication::setOrganizationName("UniPi");
  QCoreApplication::setOrganizationDomain("unipi.it");
  QCoreApplication::setApplicationName("Moka");
  QCoreApplication::setApplicationVersion("1.0.0");

  // Builds the main window and start its execution
  gui::MokaWindow moka_window;
  moka_window.show();
  
  return qapp.exec();
} // function main
