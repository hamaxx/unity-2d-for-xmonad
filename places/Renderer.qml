import Qt 4.7

/* A renderer is used to display a group of a place, that is a set of items from
   a place that share the same group id. Concretely speaking, it will be a section
   of search results, for example, "Available Applications".

   The renderer to be used for a given group is chosen by the place backend
   itself. A typical renderer is the UnityDefaultRender that lays out the items
   in a grid of icons with the item's title underneath it.
*/
Item {
    property string display_name /* Name of the group typically displayed in the header */
    property string icon_hint /* Icon id of the group */
    property variant model /* List model containing the items to be displayed by the renderer */
    property variant place /* Reference to the place the group belongs to */
    property int model_count_limit: -1 /* Maximum number of items to display; -1 for no limit */
}
