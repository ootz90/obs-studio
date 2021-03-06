/******************************************************************************
    Copyright (C) 2014 by Hugh Bailey <obs.jim@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#pragma once

#include <QDialog>
#include <memory>

#include <obs.hpp>

#include "properties-view.hpp"

class OBSBasic;

#include "ui_OBSBasicProperties.h"

class OBSBasicProperties : public QDialog {
	Q_OBJECT

private:
	OBSBasic   *main;
	int        resizeTimer;

	std::unique_ptr<Ui::OBSBasicProperties> ui;
	OBSSource  source;
	OBSDisplay display;
	OBSSignal  removedSignal;
	OBSPropertiesView *view;

	static void SourceRemoved(void *data, calldata_t params);
	static void DrawPreview(void *data, uint32_t cx, uint32_t cy);

public:
	OBSBasicProperties(QWidget *parent, OBSSource source_);

	void Init();

protected:
	virtual void closeEvent(QCloseEvent *event) override;
	virtual void resizeEvent(QResizeEvent *event) override;
	virtual void timerEvent(QTimerEvent *event) override;
};
