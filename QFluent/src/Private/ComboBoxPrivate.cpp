#include "ComboBoxPrivate.h"
#include "QFluent/ComboBox.h"

#include "Animation.h"

#include <QAbstractItemModel>
#include <QAction>
#include <QPointer>

ComboBoxPrivate::ComboBoxPrivate(ComboBox *q)
    : QObject(q)
    , q_ptr(q)
{
    m_internalModel = new ComboItemModel(this);
    m_model = m_internalModel;
    connectModel(m_model);
}

ComboBoxPrivate::~ComboBoxPrivate() = default;

void ComboBoxPrivate::setModel(QAbstractItemModel *model)
{
    Q_Q(ComboBox);

    if (!model) {
        model = m_internalModel;
    }

    if (model == m_model)
        return;

    closeComboMenu();
    disconnectModel(m_model);
    m_model = model;
    connectModel(m_model);

    int oldIndex = m_currentIndex;
    m_currentIndex = -1;
    updateTextState();

    if (oldIndex != -1) {
        emit q->currentIndexChanged(-1);
        emit q->currentTextChanged(QString());
    }
}

void ComboBoxPrivate::connectModel(QAbstractItemModel *model)
{
    if (!model)
        return;

    connect(model, &QAbstractItemModel::rowsInserted, this, &ComboBoxPrivate::onRowsInserted);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &ComboBoxPrivate::onRowsRemoved);
    connect(model, &QAbstractItemModel::modelReset, this, &ComboBoxPrivate::onModelReset);
    connect(model, &QAbstractItemModel::dataChanged, this, &ComboBoxPrivate::onDataChanged);
    if (model != m_internalModel) {
        connect(model, &QObject::destroyed, this, &ComboBoxPrivate::onModelDestroyed);
    }
}

void ComboBoxPrivate::disconnectModel(QAbstractItemModel *model)
{
    if (!model)
        return;

    disconnect(model, nullptr, this, nullptr);
}

void ComboBoxPrivate::resetModelToInternal()
{
    Q_Q(ComboBox);

    closeComboMenu();
    m_model = m_internalModel;
    connectModel(m_model);

    int oldIndex = m_currentIndex;
    m_currentIndex = -1;
    updateTextState();

    if (oldIndex != -1) {
        emit q->currentIndexChanged(-1);
        emit q->currentTextChanged(QString());
    }
}

void ComboBoxPrivate::createComboMenu()
{
    Q_Q(ComboBox);

    if (m_comboMenu) {
        closeComboMenu();
    }

    m_comboMenu = new ComboBoxMenu("menu", q);
    ComboBoxMenu *menu = m_comboMenu;

    for (int i = 0; i < m_model->rowCount(); ++i) {
        QModelIndex index = m_model->index(i, 0);
        bool isSep = m_model->data(index, ComboItemModel::SeparatorRole).toBool();

        if (isSep) {
            m_comboMenu->addSeparator();
            continue;
        }

        QString text = m_model->data(index, Qt::DisplayRole).toString();
        QIcon icon = m_model->data(index, Qt::DecorationRole).value<QIcon>();
        QAction *action = new QAction(icon, text, m_comboMenu);
        if (icon.isNull()) {
            action = new QAction(text, m_comboMenu);
        }
        m_comboMenu->addAction(action);
        action->setData(i);
        connect(action, &QAction::triggered, this, [this, i]() { onMenuAction(i); });
    }

    QPointer<ComboBox> qPtr = q;
    connect(menu, &ComboBoxMenu::closed, q, [qPtr, this, menu]() {
        if (!qPtr) return;
        if (m_comboMenu == menu) {
            m_comboMenu = nullptr;
        }
        menu->deleteLater();
    });
}

void ComboBoxPrivate::showComboMenu()
{
    Q_Q(ComboBox);

    if (m_model->rowCount() == 0) {
        return;
    }

    createComboMenu();

    if (m_currentIndex >= 0 && m_currentIndex < m_model->rowCount()) {
        for (QAction *action : m_comboMenu->menuActions()) {
            if (action->data().toInt() == m_currentIndex) {
                m_comboMenu->setDefaultAction(action);
                break;
            }
        }
    }

    ComboBoxHelper::showComboMenu(q, m_comboMenu, m_maxVisibleItems);
}

void ComboBoxPrivate::closeComboMenu()
{
    if (!m_comboMenu) {
        return;
    }
    ComboBoxMenu *menu = m_comboMenu;
    m_comboMenu = nullptr;
    menu->close();
    menu->deleteLater();
}

void ComboBoxPrivate::toggleComboMenu()
{
    if (m_comboMenu != nullptr) {
        closeComboMenu();
    } else {
        showComboMenu();
    }
}

void ComboBoxPrivate::updateTextState()
{
    Q_Q(ComboBox);

    if (m_currentIndex >= 0 && m_currentIndex < m_model->rowCount()) {
        QModelIndex index = m_model->index(m_currentIndex, 0);
        QString text = m_model->data(index, Qt::DisplayRole).toString();
        m_settingCurrentIndex = true;
        q->setText(text);
        m_settingCurrentIndex = false;
    } else {
        m_settingCurrentIndex = true;
        q->setText(m_placeholderText);
        m_settingCurrentIndex = false;
    }
}

void ComboBoxPrivate::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_Q(ComboBox);

    closeComboMenu();

    if (m_currentIndex < 0)
        return;

    if (first <= m_currentIndex) {
        int count = last - first + 1;
        m_currentIndex += count;
        emit q->currentIndexChanged(m_currentIndex);
    }
}

void ComboBoxPrivate::onRowsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_Q(ComboBox);

    closeComboMenu();

    if (m_currentIndex < 0)
        return;

    int count = last - first + 1;
    if (m_currentIndex >= first && m_currentIndex <= last) {
        m_currentIndex = -1;
        updateTextState();
        emit q->currentIndexChanged(-1);
        emit q->currentTextChanged(QString());
    } else if (m_currentIndex > last) {
        m_currentIndex -= count;
        emit q->currentIndexChanged(m_currentIndex);
    }
}

void ComboBoxPrivate::onModelReset()
{
    Q_Q(ComboBox);

    closeComboMenu();
    m_currentIndex = -1;
    updateTextState();
    emit q->currentIndexChanged(-1);
    emit q->currentTextChanged(QString());
}

void ComboBoxPrivate::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_Q(ComboBox);

    closeComboMenu();

    if (m_currentIndex >= topLeft.row() && m_currentIndex <= bottomRight.row()) {
        updateTextState();
        QModelIndex index = m_model->index(m_currentIndex, 0);
        emit q->currentTextChanged(m_model->data(index, Qt::DisplayRole).toString());
    }
}

void ComboBoxPrivate::onMenuAction(int index)
{
    Q_Q(ComboBox);
    q->setCurrentIndex(index);
    closeComboMenu();
}

void ComboBoxPrivate::onModelDestroyed(QObject *object)
{
    if (object != m_model)
        return;

    resetModelToInternal();
}