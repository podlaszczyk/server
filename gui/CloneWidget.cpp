#include "CloneWidget.h"

#include <QPainter>

CloneWidget::CloneWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(120, 32);
}

void CloneWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    QColor backgroundColor(30, 30, 30);

    int iconSize = 32;
    int iconX = 0;
    int iconY = 0;

    int textX = iconX + iconSize + 16;
    int textY = iconY + (iconSize - painter.fontMetrics().height()) / 2 + painter.fontMetrics().ascent() + 2;

    painter.fillRect(iconX, iconY, iconSize + 100, iconSize, backgroundColor);

    QIcon icon(":/images/clone_signet_big_white.png");

    icon.paint(&painter, QRect(iconX, iconY, iconSize, iconSize), Qt::AlignCenter);

    QString text = "CLONE";
    QFont font = painter.font();
    font.setPointSize(14);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    painter.setFont(font);

    painter.setPen(Qt::white);

    painter.drawText(textX, textY, text);
}
