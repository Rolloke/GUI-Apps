#ifndef HELPER_H
#define HELPER_H

#include <QLayout>
#include <QString>

QWidget* find_widget(QLayout *layout, const QString& text);
void clearLayout(QLayout *layout);

#endif // HELPER_H
