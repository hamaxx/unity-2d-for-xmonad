import Qt 4.7

Item {
    property Item target
    property int itemWidth
    property int itemHeight

    /* Scale down to fit target while preserving the aspect ratio of the item.
       Never scale up the item. */
    property double targetAspectRatio: target.width / target.height
    property double itemAspectRatio: itemWidth / itemHeight
    property bool isHorizontal: itemAspectRatio >= targetAspectRatio
    property int maxWidth: Math.min(itemWidth, target.width)
    property int maxHeight: Math.min(itemHeight, target.height)

    width: isHorizontal ? maxWidth : maxHeight * itemAspectRatio
    height: !isHorizontal ? maxHeight : maxWidth / itemAspectRatio
    x: target.x + (target.width - width) / 2
    y: target.y + (target.height - height) / 2
}
