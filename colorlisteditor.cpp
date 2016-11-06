#include <QtGui>
#include "colorlisteditor.h"
#include <stdint.h>

ColorListEditor::ColorListEditor(QWidget *widget) : QComboBox(widget)
{
    populateList();
}

QColor ColorListEditor::color() const
{
    return qvariant_cast<QColor>(itemData(currentIndex(), Qt::DecorationRole));
}

void ColorListEditor::setColor(QColor color)
{
    setCurrentIndex(findData(color, int(Qt::DecorationRole)));
}

void ColorListEditor::populateList()
{
    QStringList colorNames = QColor::colorNames();

//    for (int i = 0; i < colorNames.size(); ++i) {
//        QColor color(colorNames[i]);

//        insertItem(i, colorNames[i]);
//        setItemData(i, color, Qt::DecorationRole);
//    }
    std::vector<QString> names;
    std::vector<QColor> colors;

    names.push_back("Black");
    names.push_back("White");
    names.push_back("Red");
    names.push_back("Cyan");
    names.push_back("Violet");
    names.push_back("Green");
    names.push_back("Blue");
    names.push_back("Yellow");
    names.push_back("Orange");
    names.push_back("Brown");
    names.push_back("Light red");
    names.push_back("Grey 1");
    names.push_back("Grey 2");
    names.push_back("Light green");
    names.push_back("Light blue");
    names.push_back("Grey 3");

    colors.push_back(QColor(  0,  0,  0));
    colors.push_back(QColor(255,255,255));
    colors.push_back(QColor(136,  0,  0));
    colors.push_back(QColor(170,255,238));
    colors.push_back(QColor(204, 68,204));
    colors.push_back(QColor(  0,204, 85));
    colors.push_back(QColor(  0,  0,170));
    colors.push_back(QColor(238,238,119));
    colors.push_back(QColor(221,136, 85));
    colors.push_back(QColor(102, 68,  0));
    colors.push_back(QColor(255,119,119));
    colors.push_back(QColor( 51, 51, 51));
    colors.push_back(QColor(119,119,119));
    colors.push_back(QColor(170,255,102));
    colors.push_back(QColor(  0,136,255));
    colors.push_back(QColor(187,187,187));

    for (uint32_t i = 0; i < colors.size(); ++i) {
        insertItem(i, names[i]);
        setItemData(i, colors[i], Qt::DecorationRole);
    }

}
