
typedef struct
{
    void    (*initialize)();
    void    (*close)();
    void    (*update)();
    void    (*redraw)();

} t_applet_handlers;
