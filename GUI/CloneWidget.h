#pragma once
#include <QWidget>

class CloneWidget : public QWidget
{
public:
    explicit CloneWidget(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};
