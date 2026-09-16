#ifndef QRIBBONTEXTLAYOUT_H
#define QRIBBONTEXTLAYOUT_H

#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QRect>
#include <QString>
#include <QStringList>

namespace QRibbonTextLayout
{
    constexpr int LargeTextLineCount = 2;

    inline QString normalizedLargeText(const QString &text)
    {
        QString normalized = text;
        normalized.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
        normalized.replace(QChar('\r'), QChar('\n'));

        const int firstBreak = normalized.indexOf(QChar('\n'));
        if (firstBreak >= 0) {
            for (int i = firstBreak + 1; i < normalized.size(); ++i) {
                if (normalized.at(i) == QChar('\n')) {
                    normalized[i] = QChar(' ');
                }
            }
        }
        return normalized;
    }

    inline int preferredTwoLineWidth(const QFont &font,
                                     const QString &text,
                                     int minimumWidth = 0)
    {
        const QString normalized = normalizedLargeText(text);
        if (normalized.isEmpty()) return qMax(0, minimumWidth);

        const QFontMetrics fm(font);
        int width = qMax(0, minimumWidth);
        const QStringList lines = normalized.split(QChar('\n'));
        for (const QString &line : lines) {
            width = qMax(width, fm.horizontalAdvance(line));
        }
        return width;
    }

    inline void drawTwoLineText(QPainter &painter, const QRect &rect, const QString &text)
    {
        const QString normalized = normalizedLargeText(text);
        if (normalized.isEmpty() || !rect.isValid()) return;

        const QStringList lines = normalized.split(QChar('\n'));
        const QFontMetrics fm(painter.font());
        const int lineHeight = fm.height();
        const int lineCount = qMin(lines.size(), LargeTextLineCount);
        const int totalHeight = lineHeight * lineCount;
        int y = rect.top() + qMax(0, (rect.height() - totalHeight) / 2);

        painter.save();
        painter.setClipRect(rect);
        for (int i = 0; i < lineCount; ++i) {
            const QRect lineRect(rect.left(), y, rect.width(), lineHeight);
            painter.drawText(lineRect, Qt::AlignLeft | Qt::AlignVCenter, lines.at(i));
            y += lineHeight;
        }
        painter.restore();
    }
}

#endif // QRIBBONTEXTLAYOUT_H
