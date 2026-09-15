#ifndef QRIBBONTEXTLAYOUT_H
#define QRIBBONTEXTLAYOUT_H

#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPointF>
#include <QRect>
#include <QString>
#include <QStringList>
#include <QTextLayout>
#include <QTextOption>

namespace QRibbonTextLayout
{
    constexpr int LargeTextLineCount = 2;

    inline QString normalizedLargeText(const QString &text)
    {
        QString normalized = text;
        normalized.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
        normalized.replace(QChar('\r'), QChar('\n'));

        // Large buttons intentionally support at most two visual lines. Keep the
        // first explicit line break and turn any additional breaks into spaces.
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

    inline int wrappedLineCount(const QFont &font, const QString &text, int width)
    {
        const QString normalized = normalizedLargeText(text);
        if (normalized.isEmpty()) return 0;

        QTextLayout layout(normalized, font);
        QTextOption option;
        option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        option.setAlignment(Qt::AlignLeft);
        layout.setTextOption(option);

        int count = 0;
        layout.beginLayout();
        while (true) {
            QTextLine line = layout.createLine();
            if (!line.isValid()) break;
            line.setLineWidth(qMax(1, width));
            ++count;
        }
        layout.endLayout();
        return count;
    }

    inline int preferredTwoLineWidth(const QFont &font,
                                     const QString &text,
                                     int minimumWidth = 1)
    {
        const QString normalized = normalizedLargeText(text);
        if (normalized.isEmpty()) return qMax(1, minimumWidth);

        const QFontMetrics fm(font);
        int high = qMax(1, minimumWidth);
        const QStringList explicitLines = normalized.split(QChar('\n'));
        for (const QString &line : explicitLines) {
            high = qMax(high, fm.horizontalAdvance(line));
        }

        if (wrappedLineCount(font, normalized, high) > LargeTextLineCount) {
            return high;
        }

        int low = qMax(1, minimumWidth);
        while (low < high) {
            const int middle = low + (high - low) / 2;
            if (wrappedLineCount(font, normalized, middle) <= LargeTextLineCount) {
                high = middle;
            } else {
                low = middle + 1;
            }
        }
        return low;
    }

    inline void drawTwoLineText(QPainter &painter, const QRect &rect, const QString &text)
    {
        const QString normalized = normalizedLargeText(text);
        if (normalized.isEmpty() || !rect.isValid()) return;

        QTextLayout layout(normalized, painter.font());
        QTextOption option;
        option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        option.setAlignment(Qt::AlignLeft);
        layout.setTextOption(option);

        qreal y = 0.0;
        int lineCount = 0;
        layout.beginLayout();
        while (lineCount < LargeTextLineCount) {
            QTextLine line = layout.createLine();
            if (!line.isValid()) break;
            line.setLineWidth(qMax(1, rect.width()));
            line.setPosition(QPointF(0.0, y));
            y += line.height();
            ++lineCount;
        }
        layout.endLayout();

        painter.save();
        painter.setClipRect(rect);
        layout.draw(&painter, rect.topLeft());
        painter.restore();
    }
}

#endif // QRIBBONTEXTLAYOUT_H
