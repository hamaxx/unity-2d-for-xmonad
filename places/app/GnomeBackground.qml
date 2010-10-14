import Qt 4.7
import gconf 1.0

Item {

    GConfItem {
        id: primary_color
        key: "/desktop/gnome/background/primary_color"
    }

    GConfItem {
        id: picture_filename
        key: "/desktop/gnome/background/picture_filename"
    }

    GConfItem {
        id: picture_options
        key: "/desktop/gnome/background/picture_options"
    }

    Rectangle {
        anchors.fill: parent
        color: primary_color.value
        visible: picture.opacity != 1.0
    }

    Image {
        id: picture

        anchors.fill: parent
        opacity: picture_filename.value ? 1.0 : 0.0
        source: picture_filename.value
        sourceSize.width: width
        sourceSize.height: height
        /* Possible modes are:
            - "wallpaper"
            - "centered" (NOT IMPLEMENTED)
            - "scaled"
            - "stretched"
            - "zoom"
            - "spanned" (NOT IMPLEMENTED)
        */
        fillMode: {
            if(picture_options.value == "wallpaper")
                return Image.Tile
            else if(picture_options.value == "scaled")
                return Image.PreserveAspectFit
            else if(picture_options.value == "stretched")
                return Image.Stretch
            else if(picture_options.value == "zoom")
                return Image.PreserveAspectCrop
            else return Image.Tile
        }
    }
}
