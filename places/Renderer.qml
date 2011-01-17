import Qt 4.7

/* A renderer is used to display a group of a place, that is a set of items from
   a place that share the same group id. Concretely speaking, it will be a section
   of search results, for example, "Available Applications".

   The renderer to be used for a given group is chosen by the place backend
   itself. A typical renderer is the UnityDefaultRender that lays out the items
   in a grid of icons with the item's title underneath it.
*/
Item {
    property string displayName /* Name of the group typically displayed in the header */
    property string iconHint /* Icon id of the group */
    property int groupId /* Index of the group */
    property variant model /* List model containing the items to be displayed by the renderer */
    property variant placeEntryModel /* Reference to the place entry the group belongs to */
    property variant parentListView /* Reference to the ListView the renderer is nested into */
}
