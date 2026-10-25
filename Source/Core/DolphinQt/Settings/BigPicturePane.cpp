// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Settings/BigPicturePane.h"

#include <array>
#include <memory>

#include <QPushButton>

#include "ui_BigPicturePane.h"

BigPicturePane::BigPicturePane(QWidget* parent)
    : QWidget(parent), m_ui(std::make_unique<Ui::BigPicturePane>())
{
  m_ui->setupUi(this);
  const std::array buttons = {m_ui->configureController1Button, m_ui->configureController2Button,
                              m_ui->configureController3Button, m_ui->configureController4Button};
  for (int controller = 0; controller < static_cast<int>(buttons.size()); ++controller)
    connect(buttons[controller], &QPushButton::clicked, this,
            [this, controller] { emit ConfigureInput(controller); });
}

BigPicturePane::~BigPicturePane() = default;
