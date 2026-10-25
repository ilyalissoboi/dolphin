// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>

#include <QWidget>

namespace Ui
{
class BigPicturePane;
}

class BigPicturePane final : public QWidget
{
  Q_OBJECT
public:
  explicit BigPicturePane(QWidget* parent = nullptr);
  ~BigPicturePane() override;

signals:
  void ConfigureInput(int controller);

private:
  std::unique_ptr<Ui::BigPicturePane> m_ui;
};
