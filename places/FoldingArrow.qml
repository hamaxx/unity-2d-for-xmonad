import Qt 4.7
import Effects 1.0

Image {
    property bool folded: true

    rotation: folded ? 0 : 90
    source: "artwork/arrow.png"
    width: sourceSize.width
    height: sourceSize.height
    fillMode: Image.PreserveAspectFit

    Behavior on rotation {NumberAnimation {duration: 100}}
}
