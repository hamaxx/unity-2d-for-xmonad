import Qt 4.7
import gconf 1.0
/* Necessary to access the blended image provider */
import UnityPlaces 1.0

Item {
    property string overlay_color
    property real overlay_alpha

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
        Rectangle {
            anchors.fill: parent
            opacity: overlay_alpha
            color: overlay_color
        }

        anchors.fill: parent
        color: primary_color.value
        visible: !picture.visible
    }

    Image {
        id: picture

        visible: picture_filename.value
        source: {
            /* FIXME: Because /usr/share/backgrounds/warty-final-ubuntu.png is
                      actually a jpeg and Qt relies by default on the extension
                      that particular background fails to load. We workaround
                      it by having our own symlink with a 'jpg' extension.

               References:
               https://bugs.launchpad.net/ubuntu/+source/ubuntu-wallpapers/+bug/296538
               http://bugreports.qt.nokia.com/browse/QTBUG-7276
            */
            var filename = picture_filename.value
            if(filename == "/usr/share/backgrounds/warty-final-ubuntu.png")
                filename = engineBaseUrl + "artwork/warty-final-ubuntu.jpg"

            if(overlay_alpha > 0.0)
                return "image://blended/%1color=%2alpha=%3".arg(filename).arg(overlay_color).arg(overlay_alpha)
            else
                return filename
        }
        width: screenGeometry.width
        height: screenGeometry.height
        sourceSize.width: width
        x: -availableGeometry.x
        y: -availableGeometry.y

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
