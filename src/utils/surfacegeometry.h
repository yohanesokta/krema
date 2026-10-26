// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Krema Contributors

#pragma once

#include <algorithm>
#include <cmath>

namespace krema
{

/// Extra height above the panel needed for zoomed icons.
inline int zoomOverflowHeight(int iconSize, double maxZoomFactor)
{
    return static_cast<int>(std::ceil(iconSize * (maxZoomFactor - 1.0)));
}

/// Ground-space shadow overflow along the dock's secondary axis.
inline int shadowOverflowHeight(int panelExtent, int lightOffset, int lightZ, int elevation, double lightRadius)
{
    const double denominator = std::max(lightZ - elevation, 1);
    const double projection = static_cast<double>(elevation) / denominator;
    const double panelOverhang = (std::max(panelExtent, 0) / 2.0 + std::abs(lightOffset)) * projection;
    const double softness = 3.0 * std::max(lightRadius, 0.5) * lightZ / denominator;
    return static_cast<int>(std::ceil(panelOverhang + softness + 10.0));
}

/// Extra reserve needed while a zoomed icon performs its largest bounce.
///
/// DockItem scales its complete icon cell (the icon plus the fixed indicator
/// cell) around the screen-edge side, then applies the child bounce translate.
/// Both the transformed cell extent and the largest attention bounce therefore
/// have to be included in the outward reserve.  The indicator cell mirrors
/// DockItem's `4 + Kirigami.Units.smallSpacing` (8 px on supported themes).
inline int animationOverflowHeight(int iconSize, int padding, double maxZoomFactor)
{
    constexpr double indicatorSpace = 8.0;
    constexpr double maxBounceDistance = 14.0;
    const double scale = std::max(1.0, maxZoomFactor);
    const double transformedCellGrowth = (iconSize + indicatorSpace) * (scale - 1.0);
    const double transformedBounce = maxBounceDistance * scale;
    const double reserve = transformedCellGrowth + transformedBounce - padding;
    // Avoid promoting an exactly integral geometric result because of the
    // last-bit error in the floating-point multiplication above.
    return std::max(0, static_cast<int>(std::ceil(reserve - 1e-9)));
}

/// Total surface height including dock, inward animation / tooltip / shadow reserve,
/// outward floating reserve, and the dock content.
inline int surfaceHeight(int iconSize, int paddingTop, int paddingBottom, double maxZoomFactor, int tooltipReserve, int floatingPadding, int shadowOverflow = 0)
{
    const int dockHeight = iconSize + paddingTop + paddingBottom;
    const int overflow = animationOverflowHeight(iconSize, paddingTop, maxZoomFactor);

    const int topOverflow = std::max({overflow, tooltipReserve, shadowOverflow});
    const int bottomOverflow = floatingPadding;

    return dockHeight + topOverflow + bottomOverflow;
}

inline int surfaceHeight(int iconSize, int padding, double maxZoomFactor, int tooltipReserve, int floatingPadding, int shadowOverflow = 0)
{
    return surfaceHeight(iconSize, padding, padding, maxZoomFactor, tooltipReserve, floatingPadding, shadowOverflow);
}

/// Height of the visible panel bar (icon + padding + floating gap).
inline int panelBarHeight(int iconSize, int paddingTop, int paddingBottom, int floatingPadding)
{
    return iconSize + paddingTop + paddingBottom + floatingPadding;
}

inline int panelBarHeight(int iconSize, int padding, int floatingPadding)
{
    return panelBarHeight(iconSize, padding, padding, floatingPadding);
}

/// Screen-space reservation needed when the panel is placed at floatingPadding.
inline int screenReserveDepth(int iconSize, int paddingTop, int paddingBottom, int floatingPadding, int shadowOverflow, int reservedInset)
{
    (void)shadowOverflow;
    const int panelEdgeOffset = floatingPadding;
    return iconSize + paddingTop + paddingBottom + panelEdgeOffset - reservedInset;
}

inline int screenReserveDepth(int iconSize, int padding, int floatingPadding, int shadowOverflow, int reservedInset)
{
    return screenReserveDepth(iconSize, padding, padding, floatingPadding, shadowOverflow, reservedInset);
}

} // namespace krema
