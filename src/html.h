
struct html_tag {
   unsigned char length;
   char *entity;
};

#define NUM_OF_SKIP_TAGS2 10

struct html_tag skip_html_tags2[] = {
   { 4, "html" },
   { 5, "/html" },
   { 5, "/body" },
   { 4, "meta" },
   { 4, "head" },
   { 5, "/head" },
   { 5, "style" },
   { 6, "/style" },
   { 3, "div" },
   { 4, "/div" }
};


#define NUM_OF_SKIP_TAGS 11

struct html_tag skip_html_tags[] = {
   { 5, "style" },
   { 4, "dir=" },
   { 8, "content=" },
   { 5, "name=" },
   { 3, "id=" },
   { 2, "v:" },
   { 6, "class=" },
   { 5, "xmlns" },
   { 10, "http-equiv" },
   { 7, "spidmax" },
   { 5, "data=" }
};

