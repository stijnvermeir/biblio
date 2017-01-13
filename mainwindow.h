#pragma once

#include "bibtablemodel.h"

#include <QMainWindow>
#include <QCompleter>
#include <QStringListModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;
	BibTableModel model_;
	QCompleter completer_;
	QStringListModel* completerModel_;
};
