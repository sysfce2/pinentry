/* pinentryconfirm.cpp - A QMessageBox with a timeout
 *
 * Copyright (C) 2011 Ben Kibbey <bjk@luxsci.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 * SPDX-License-Identifier: GPL-2.0+
 */

#include "pinentryconfirm.h"

#include "accessibility.h"
#include "pinentrydialog.h"

#include <QAbstractButton>
#include <QGridLayout>
#include <QSpacerItem>
#include <QFontMetrics>

PinentryConfirm::PinentryConfirm(Icon icon, const QString &title, const QString &text,
                                 StandardButtons buttons, QWidget *parent, Qt::WindowFlags flags)
    : QMessageBox{icon, title, text, buttons, parent, flags}
{
    _timer.callOnTimeout(this, &PinentryConfirm::slotTimeout);
    Accessibility::setDescription(this, text);
    Accessibility::setName(this, title);
    raiseWindow(this);
}

void PinentryConfirm::setTimeout(std::chrono::seconds timeout)
{
    _timer.setInterval(timeout);
}

std::chrono::seconds PinentryConfirm::timeout() const
{
    return std::chrono::duration_cast<std::chrono::seconds>(_timer.intervalAsDuration());
}

bool PinentryConfirm::timedOut() const
{
    return _timed_out;
}

void PinentryConfirm::showEvent(QShowEvent *event)
{
    static bool resized;
    if (!resized) {
        QGridLayout* lay = dynamic_cast<QGridLayout*> (layout());
        if (lay) {
            QSize textSize = fontMetrics().size(Qt::TextExpandTabs, text(), fontMetrics().maxWidth());
            QSpacerItem* horizontalSpacer = new QSpacerItem(textSize.width() + iconPixmap().width(),
                                                            0, QSizePolicy::Minimum, QSizePolicy::Expanding);
            lay->addItem(horizontalSpacer, lay->rowCount(), 1, 1, lay->columnCount() - 1);
        }
        resized = true;
    }

    QMessageBox::showEvent(event);
    raiseWindow(this);

    if (timeout() > std::chrono::milliseconds::zero()) {
        _timer.setSingleShot(true);
        _timer.start();
    }
}

void PinentryConfirm::slotTimeout()
{
    QAbstractButton *b = button(QMessageBox::Cancel);
    _timed_out = true;

    if (b) {
        b->animateClick(0);
    }
}

#include "pinentryconfirm.moc"
