#include "QFluent/MultiViewComboBox.h"

#include <QtTest/QtTest>

class MultiViewComboBoxTest : public QObject
{
    Q_OBJECT

private slots:
    void removingEarlierRowKeepsSelectionOnSameItem();
};

void MultiViewComboBoxTest::removingEarlierRowKeepsSelectionOnSameItem()
{
    MultiViewComboBox combo;
    combo.addItem("A", 1);
    combo.addItem("B", 2);
    combo.addItem("C", 3);

    combo.setItemSelected(2, true);
    QCOMPARE(combo.selectedIndexes(), QList<int>({2}));
    QCOMPARE(combo.selectedTexts(), QStringList({"C"}));
    QCOMPARE(combo.selectedDatas(), QList<QVariant>({QVariant(3)}));

    combo.removeItem(0);

    QCOMPARE(combo.selectedIndexes(), QList<int>({1}));
    QCOMPARE(combo.selectedTexts(), QStringList({"C"}));
    QCOMPARE(combo.selectedDatas(), QList<QVariant>({QVariant(3)}));
}

QTEST_MAIN(MultiViewComboBoxTest)
#include "tst_multiviewcombobox.moc"
