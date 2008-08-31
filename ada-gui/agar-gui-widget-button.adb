package body agar.gui.widget.button is

  function c_allocate
    (widget : widget_access_t;
     flags  : flags_t;
     label  : cs.chars_ptr) return button_access_t;
  pragma import (c, c_allocate, "AG_ButtonNew");

  function c_allocate_function
    (widget : widget_access_t;
     flags  : flags_t;
     label  : cs.chars_ptr;
     func   : agar.core.event.callback_t;
     fmt    : agar.core.types.void_ptr_t) return button_access_t;
  pragma import (c, c_allocate_function, "AG_ButtonNewFn");

  function c_allocate_integer
    (widget : widget_access_t;
     flags  : flags_t;
     label  : cs.chars_ptr;
     ptr    : access c.int) return button_access_t;
  pragma import (c, c_allocate_integer, "agar_gui_widget_new_int");
 
  function c_allocate_uint8
    (widget : widget_access_t;
     flags  : flags_t;
     label  : cs.chars_ptr;
     ptr    : access agar.core.types.uint8_t) return button_access_t;
  pragma import (c, c_allocate_uint8, "agar_gui_widget_new_uint8");

  function c_allocate_uint16
    (widget : widget_access_t;
     flags  : flags_t;
     label  : cs.chars_ptr;
     ptr    : access agar.core.types.uint16_t) return button_access_t;
  pragma import (c, c_allocate_uint16, "agar_gui_widget_new_uint16");

  function c_allocate_uint32
    (widget : widget_access_t;
     flags  : flags_t;
     label  : cs.chars_ptr;
     ptr    : access agar.core.types.uint32_t) return button_access_t;
  pragma import (c, c_allocate_uint32, "agar_gui_widget_new_uint32");

  function c_allocate_flag
    (widget : widget_access_t;
     flags  : flags_t;
     label  : cs.chars_ptr;
     ptr    : access c.int;
     mask   : c.unsigned) return button_access_t;
  pragma import (c, c_allocate_flag, "AG_ButtonNewFlag");

  function c_allocate_flag8
    (widget : widget_access_t;
     flags  : flags_t;
     label  : cs.chars_ptr;
     ptr    : access agar.core.types.uint8_t;
     mask   : agar.core.types.uint8_t) return button_access_t;
  pragma import (c, c_allocate_flag8, "AG_ButtonNewFlag8");

  function c_allocate_flag16
    (widget : widget_access_t;
     flags  : flags_t;
     label  : cs.chars_ptr;
     ptr    : access agar.core.types.uint16_t;
     mask   : agar.core.types.uint16_t) return button_access_t;
  pragma import (c, c_allocate_flag16, "AG_ButtonNewFlag16");

  function c_allocate_flag32
    (widget : widget_access_t;
     flags  : flags_t;
     label  : cs.chars_ptr;
     ptr    : access agar.core.types.uint32_t;
     mask   : agar.core.types.uint32_t) return button_access_t;
  pragma import (c, c_allocate_flag32, "AG_ButtonNewFlag32");

  procedure c_set_padding 
    (button     : button_access_t; 
     left_pad   : c.int; 
     right_pad  : c.int; 
     top_pad    : c.int; 
     bottom_pad : c.int); 
  pragma import (c, c_set_padding, "AG_ButtonSetPadding"); 

  procedure c_set_focusable
    (button : button_access_t;
     flag   : c.int); 
  pragma import (c, c_set_focusable, "AG_ButtonSetFocusable"); 

  procedure c_set_sticky
    (button : button_access_t;
     flag   : c.int); 
  pragma import (c, c_set_sticky, "AG_ButtonSetSticky"); 

  procedure c_set_repeat_mode
    (button : button_access_t;
     flag   : c.int); 
  pragma import (c, c_set_repeat_mode, "AG_ButtonSetRepeatMode"); 

  procedure c_invert_state
    (button : button_access_t;
     flag   : c.int); 
  pragma import (c, c_invert_state, "AG_ButtonInvertState"); 

  procedure c_text 
    (button : button_access_t; 
     fmt    : cs.chars_ptr;
     text   : cs.chars_ptr); 
  pragma import (c, c_text, "AG_ButtonText");

  procedure c_text_no_copy
    (button : button_access_t; 
     fmt    : cs.chars_ptr;
     text   : cs.chars_ptr); 
  pragma import (c, c_text_no_copy, "AG_ButtonTextNODUP");

  --

  function allocate
    (widget : widget_access_t;
     flags  : flags_t;
     label  : string) return button_access_t
  is
    c_label : aliased c.char_array := c.to_c (label);
  begin
    return c_allocate
      (widget => widget,
       flags  => flags,
       label  => cs.to_chars_ptr (c_label'unchecked_access));
  end allocate;

  function allocate_function
    (widget : widget_access_t;
     flags  : flags_t;
     label  : string;
     func   : agar.core.event.callback_t) return button_access_t
  is
    c_label : aliased c.char_array := c.to_c (label);
  begin
    return c_allocate_function
      (widget => widget,
       func   => func,
       flags  => flags,
       label  => cs.to_chars_ptr (c_label'unchecked_access),
       fmt    => agar.core.types.null_ptr);
  end allocate_function;

  function allocate_integer
    (widget : widget_access_t;
     flags  : flags_t;
     label  : string;
     ptr    : access integer) return button_access_t
  is
    c_label  : aliased c.char_array := c.to_c (label);
    c_int    : aliased c.int;
    c_button : button_access_t;
  begin
    c_button := c_allocate_integer
      (widget => widget,
       flags  => flags,
       label  => cs.to_chars_ptr (c_label'unchecked_access),
       ptr    => c_int'unchecked_access);
    if c_button /= null then ptr.all := integer (c_int); end if;
    return c_button;
  end allocate_integer;

  function allocate_uint8
    (widget : widget_access_t;
     flags  : flags_t;
     label  : string;
     ptr    : access agar.core.types.uint8_t) return button_access_t
  is
    c_label   : aliased c.char_array := c.to_c (label);
  begin
    return c_allocate_uint8
      (widget => widget,
       flags  => flags,
       label  => cs.to_chars_ptr (c_label'unchecked_access),
       ptr    => ptr);
  end allocate_uint8;
 
  function allocate_uint16
    (widget : widget_access_t;
     flags  : flags_t;
     label  : string;
     ptr    : access agar.core.types.uint16_t) return button_access_t
  is
    c_label   : aliased c.char_array := c.to_c (label);
  begin
    return c_allocate_uint16
      (widget => widget,
       flags  => flags,
       label  => cs.to_chars_ptr (c_label'unchecked_access),
       ptr    => ptr);
  end allocate_uint16;
 
  function allocate_uint32
    (widget : widget_access_t;
     flags  : flags_t;
     label  : string;
     ptr    : access agar.core.types.uint32_t) return button_access_t
  is
    c_label   : aliased c.char_array := c.to_c (label);
  begin
    return c_allocate_uint32
      (widget => widget,
       flags  => flags,
       label  => cs.to_chars_ptr (c_label'unchecked_access),
       ptr    => ptr);
  end allocate_uint32;
 
  function allocate_flag
    (widget : widget_access_t;
     flags  : flags_t;
     label  : string;
     ptr    : access integer;
     mask   : integer) return button_access_t
  is
    c_label  : aliased c.char_array := c.to_c (label);
    c_int    : aliased c.int;
    c_button : aliased button_access_t;
  begin
    c_button := c_allocate_flag
      (widget => widget,
       flags  => flags,
       label  => cs.to_chars_ptr (c_label'unchecked_access),
       ptr    => c_int'unchecked_access,
       mask   => c.unsigned (mask));
    if c_button /= null then ptr.all := integer (c_int); end if;
    return c_button;
  end allocate_flag;

  function allocate_flag8
    (widget : widget_access_t;
     flags  : flags_t;
     label  : string;
     ptr    : access agar.core.types.uint8_t;
     mask   : agar.core.types.uint8_t) return button_access_t
  is
    c_label : aliased c.char_array := c.to_c (label);
  begin
    return c_allocate_flag8
      (widget => widget,
       flags  => flags,
       label  => cs.to_chars_ptr (c_label'unchecked_access),
       ptr    => ptr,
       mask   => mask);
  end allocate_flag8;
 
  function allocate_flag16
    (widget : widget_access_t;
     flags  : flags_t;
     label  : string;
     ptr    : access agar.core.types.uint16_t;
     mask   : agar.core.types.uint16_t) return button_access_t
  is
    c_label   : aliased c.char_array := c.to_c (label);
  begin
    return c_allocate_flag16
      (widget => widget,
       flags  => flags,
       label  => cs.to_chars_ptr (c_label'unchecked_access),
       ptr    => ptr,
       mask   => mask);
  end allocate_flag16;
 
  function allocate_flag32
    (widget : widget_access_t;
     flags  : flags_t;
     label  : string;
     ptr    : access agar.core.types.uint32_t;
     mask   : agar.core.types.uint32_t) return button_access_t
  is
    c_label   : aliased c.char_array := c.to_c (label);
  begin
    return c_allocate_flag32
      (widget => widget,
       flags  => flags,
       label  => cs.to_chars_ptr (c_label'unchecked_access),
       ptr    => ptr,
       mask   => mask);
  end allocate_flag32;

  procedure set_padding 
    (button     : button_access_t; 
     left_pad   : natural; 
     right_pad  : natural; 
     top_pad    : natural; 
     bottom_pad : natural) is
  begin
    c_set_padding
      (button     => button,
       left_pad   => c.int (left_pad),
       right_pad  => c.int (right_pad),
       top_pad    => c.int (top_pad),
       bottom_pad => c.int (bottom_pad));
  end set_padding;

  procedure set_focusable
    (button : button_access_t;
     flag   : boolean) is
  begin
    if flag then
      c_set_focusable (button, 1);
    else
      c_set_focusable (button, 0);
    end if;
  end set_focusable;

  procedure set_sticky
    (button : button_access_t;
     flag   : boolean) is
  begin
    if flag then
      c_set_sticky (button, 1);
    else
      c_set_sticky (button, 0);
    end if;
  end set_sticky;

  procedure invert_state
    (button : button_access_t;
     flag   : boolean) is
  begin
    if flag then
      c_invert_state (button, 1);
    else
      c_invert_state (button, 0);
    end if;
  end invert_state;

  procedure set_repeat_mode
    (button : button_access_t;
     flag   : boolean) is
  begin
    if flag then
      c_set_repeat_mode (button, 1);
    else
      c_set_repeat_mode (button, 0);
    end if;
  end set_repeat_mode;

  procedure text 
    (button  : button_access_t; 
     text    : string)
  is
    ca_fmt  : aliased c.char_array := c.to_c ("%s");
    ca_text : aliased c.char_array := c.to_c (text);
  begin
    c_text
      (button => button,
       fmt    => cs.to_chars_ptr (ca_fmt'unchecked_access),
       text   => cs.to_chars_ptr (ca_text'unchecked_access));
  end text;

  procedure text_no_copy 
    (button  : button_access_t; 
     text    : string)
  is
    ca_fmt  : aliased c.char_array := c.to_c ("%s");
    ca_text : aliased c.char_array := c.to_c (text);
  begin
    c_text_no_copy
      (button => button,
       fmt    => cs.to_chars_ptr (ca_fmt'unchecked_access),
       text   => cs.to_chars_ptr (ca_text'unchecked_access));
  end text_no_copy;

end agar.gui.widget.button;