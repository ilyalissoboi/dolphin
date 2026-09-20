// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/EmulationStatusWidget.h"

#include <cmath>

#include <QHBoxLayout>
#include <QLabel>

namespace
{
QLabel* CreateStatusLabel(QWidget* parent, const char* object_name)
{
  auto* const label = new QLabel(parent);
  label->setObjectName(QLatin1String{object_name});
  label->setAlignment(Qt::AlignCenter);
  label->setContentsMargins(10, 0, 10, 0);
  label->setFixedHeight(20);
  label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  return label;
}

void SetMetric(QLabel* label, const QString& text, bool available)
{
  label->setText(text);
  label->setVisible(available);
}
}  // namespace

EmulationStatusWidget::EmulationStatusWidget(QWidget* parent) : QWidget(parent)
{
  auto* const layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  m_renderer = CreateStatusLabel(this, "statusRenderer");
  m_resolution = CreateStatusLabel(this, "statusResolution");
  m_fps = CreateStatusLabel(this, "statusFps");
  m_vps = CreateStatusLabel(this, "statusVps");
  m_speed = CreateStatusLabel(this, "statusSpeed");
  m_volume = CreateStatusLabel(this, "statusVolume");

  layout->addWidget(m_renderer);
  layout->addWidget(m_resolution);
  layout->addWidget(m_fps);
  layout->addWidget(m_vps);
  layout->addWidget(m_speed);
  layout->addWidget(m_volume);

  hide();
}

void EmulationStatusWidget::SetStatus(const EmulationStatus& status)
{
  if (!status.active)
  {
    hide();
    return;
  }

  SetMetric(m_renderer, tr("Renderer: %1").arg(status.renderer), !status.renderer.isEmpty());
  SetMetric(m_resolution, tr("Resolution: %1x%2").arg(status.width).arg(status.height),
            status.width != 0 && status.height != 0);
  SetMetric(m_fps, tr("FPS: %1").arg(status.fps, 0, 'f', 1), status.fps > 0.0);
  SetMetric(m_vps, tr("VPS: %1").arg(status.vps, 0, 'f', 1), status.vps > 0.0);
  SetMetric(m_speed, tr("Speed: %1%").arg(std::lround(status.speed * 100.0)), status.speed > 0.0);
  m_volume->setText(status.muted ? tr("Volume: Muted") : tr("Volume: %1%").arg(status.volume));
  m_volume->show();
  show();
}
