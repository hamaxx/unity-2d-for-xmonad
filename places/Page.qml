import Qt 4.7

Item {
    property string name
    property bool active: false
    property int activeSection
    property variant sections
    property string searchQuery
    /* globalSearchQuery has the same value for all Pages */
    property string globalSearchQuery: pages.globalSearchQuery
}
