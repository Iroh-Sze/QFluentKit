#pragma once

#include <QLayout>
#include "FluentGlobal.h"

class QFLUENT_EXPORT ExpandLayout : public QLayout
{
    Q_OBJECT

public:
    explicit ExpandLayout(QWidget *parent = nullptr);
    ~ExpandLayout();

    void addWidget(QWidget *widget);
    void addItem(QLayoutItem *item) override;
    int count() const override;
    QLayoutItem *itemAt(int index) const override;
    QLayoutItem *takeAt(int index) override;

    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;

    void setGeometry(const QRect &rect) override;
    QSize sizeHint() const override;
    QSize minimumSize() const override;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void onWidgetDestroyed(QObject *obj);
    int doLayout(const QRect &rect, bool move) const;

    mutable QList<QLayoutItem*> m_items;
    // Parallel list for layout/event-filter; kept in sync on takeAt/destroyed
    // because nested ExpandLayout is not the parent widget's top-level layout,
    // so QWidget::~QWidget → parent->layout()->removeWidget() never reaches us.
    mutable QList<QWidget*> m_widgets;
};
