------------------------------------------------------------------------------
--                             AGAR GUI LIBRARY                             --
--                            A G A R .  T E X T                            --
--                                  S p e c                                 --
------------------------------------------------------------------------------
with Ada.Containers.Indefinite_Vectors;
with Interfaces; use Interfaces;
with Interfaces.C;
with Interfaces.C.Pointers;
with Interfaces.C.Strings;
with Agar.Types; use Agar.Types;
with Agar.Object;
with Agar.Surface;
with Agar.Widget;
with System;

package Agar.Text is
  package C renames Interfaces.C;
  package CS renames Interfaces.C.Strings;
  package SU renames Agar.Surface;
  package WID renames Agar.Widget;

  use type C.int;
  use type C.unsigned;

  TEXT_STATES_MAX : constant C.unsigned := $AG_TEXT_STATES_MAX;
  FONT_BOLD       : constant C.unsigned := 16#01#;
  FONT_ITALIC     : constant C.unsigned := 16#02#;
  FONT_UNDERLINE  : constant C.unsigned := 16#04#;
  FONT_UPPERCASE  : constant C.unsigned := 16#08#;

  -----------------------------------
  -- Horizontal Justification Mode --
  -----------------------------------
  type AG_Text_Justify is
    (LEFT,
     CENTER,
     RIGHT);
  for AG_Text_Justify use
    (LEFT   => 0,
     CENTER => 1,
     RIGHT  => 2);
  for AG_Text_Justify'Size use C.int'Size;
  
  -----------------------------
  -- Vertical Alignment Mode --
  -----------------------------
  type AG_Text_Valign is
    (TOP,
     MIDDLE,
     BOTTOM);
  for AG_Text_Valign use
    (TOP    => 0,
     MIDDLE => 1,
     BOTTOM => 2);
  for AG_Text_Valign'Size use C.int'Size;

  --------------------------------
  -- Type of message to display --
  --------------------------------
  type AG_Text_Message_Title is
    (ERROR,                       -- Error message alert
     WARNING,                     -- Warning (ignorable)
     INFO);                       -- Informational message (ignorable)
  for AG_Text_Message_Title use
    (ERROR   => 0,
     WARNING => 1,
     INFO    => 2);
  for AG_Text_Message_Title'Size use C.int'Size;

  ------------------
  -- Type of font --
  ------------------
  type AG_Font_Type is
    (VECTOR,                      -- Vector font engine (e.g., FreeType)
     BITMAP,                      -- Bitmap font engine (builtin)
     DUMMY);                      -- Null font engine
  for AG_Font_Type use
    (VECTOR => 0,
     BITMAP => 1,
     DUMMY  => 2);
  for AG_Font_Type'Size use C.int'Size;
  
  -------------------------------------------
  -- Type of data source to load font from --
  -------------------------------------------
  type AG_Font_Spec_Source is
    (FONT_FILE,                       -- Load font from file
     FONT_IN_MEMORY);                 -- Deserialize in-memory font data
  for AG_Font_Spec_Source use
    (FONT_FILE      => 0,
     FONT_IN_MEMORY => 1);
  for AG_Font_Spec_Source'Size use C.int'Size;
 
  ----------------------------
  -- Size of font in points --
  ----------------------------
#if HAVE_FLOAT
  subtype AG_Font_Points is C.double;
#else
  subtype AG_Font_Points is C.int;
#end if;
  type Font_Points_Access is access all AG_Font_Points with Convention => C;

  ----------------------
  -- Filename of font --
  ----------------------
  type AG_Font_Source_Filename is array (1 .. $AG_FILENAME_MAX) of
    aliased C.char with Convention => C;
 
  -----------------------------
  -- Agar font specification --
  -----------------------------
  type AG_Font_Spec
    (Spec_Source : AG_Font_Spec_Source := FONT_FILE) is
  record
    Size        : AG_Font_Points;              -- Font size in points
    Index       : C.int;                       -- Font index (FC_INDEX)
    Font_Type   : AG_Font_Type;                -- Font engine
    Font_Source : AG_Font_Spec_Source;         -- Source type
    C_Pad1      : Interfaces.Unsigned_32;
#if HAVE_FLOAT
    Matrix_XX   : C.double;       -- 1 --      -- Transformation matrix
    Matrix_XY   : C.double;       -- 0 --
    Matrix_YX   : C.double;       -- 0 --
    Matrix_YY   : C.double;       -- 1 --
#end if;
    case Spec_Source is
    when FONT_FILE =>
      File_Source   : AG_Font_Source_Filename;   -- Source font file
    when FONT_IN_MEMORY =>
      Memory_Source : access Unsigned_8;         -- Source memory region
      Memory_Size   : AG_Size;                   -- Size in bytes
#if AG_MODEL = AG_MEDIUM
      C_Pad2        : Interfaces.Unsigned_32;
#end if;
    end case;
  end record
    with Convention => C;
  pragma Unchecked_Union (AG_Font_Spec);

  type Font_Spec_Access is access all AG_Font_Spec with Convention => C;

  ------------------
  -- An Agar Font --
  ------------------
  type AG_Font;
  type Font_Access is access all AG_Font with Convention => C;
  subtype Font_not_null_Access is not null Font_Access;
  type AG_Font_Entry is limited record
    Next : Font_Access;
    Prev : access Font_Access;
  end record
    with Convention => C;
  type AG_Font_Bitmap_Spec is array (1 .. 28) of aliased C.char
    with Convention => C;

  type AG_Font is limited record
    Super         : aliased Agar.Object.Object;  -- [Font]
    Spec          : aliased AG_Font_Spec;        -- Font specification
    Flags         : C.unsigned;                  -- Options
    Height        : C.int;                       -- Height in pixels
    Ascent        : C.int;                       -- Ascent relative to baseline
    Descent       : C.int;                       -- Descent relative to baseline
    Line_Skip     : C.int;                       -- Multiline Y-increment

    Bitmap_Spec        : aliased AG_Font_Bitmap_Spec; -- Bitmap font spec
    TTF                : System.Address;              -- TODO TTF interface
    Bitmap_Glyphs      : System.Address;              -- TODO Bitmap glyph array
    Bitmap_Glyph_Count : C.unsigned;                  -- Bitmap glyph count
    Char_0, Char_1     : AG_Char;                     -- Bitmap font spec

    Reference_Count    : C.unsigned;             -- Reference count for cache
    Entry_in_Cache     : AG_Font_Entry;          -- Entry in cache
  end record
    with Convention => C;

  ----------------------------------
  -- A rendered (in-memory) glyph --
  ----------------------------------
  type AG_Glyph;
  type Glyph_Access is access all AG_Glyph with Convention => C;
  subtype Glyph_not_null_Access is not null Glyph_Access;
  type AG_Glyph_Entry is limited record
    Next : Glyph_Access;
  end record
    with Convention => C;
  type AG_Glyph is limited record
    Font           : Font_not_null_Access;       -- Back pointer to font
    Color          : SU.AG_Color;                -- Base color
#if AG_MODEL = AG_MEDIUM
    C_Pad1         : Interfaces.Unsigned_32;
#end if;
    Surface        : SU.Surface_not_null_Access; -- Rendered surface
    Char           : AG_Char;                    -- Native character
    Advance        : C.int;                      -- Advance in pixels
    Texture        : C.unsigned;                 -- Mapped texture (by driver)
    Texcoords      : SU.AG_Texcoord;             -- Texture coordinates
    C_Pad2         : Interfaces.Unsigned_32;
    Entry_in_Cache : AG_Glyph_Entry;             -- Entry in cache
  end record
    with Convention => C;

  ---------------------------------------
  -- Pushable/poppable state variables --
  ---------------------------------------
#if AG_DEBUG
  type AG_Text_State_Tag is array (1 .. 8) of aliased C.char
    with Convention => C;
#end if;
  type AG_Text_State is record
    Font     : Font_not_null_Access;    -- Font face
    Color    : SU.AG_Color;             -- Foreground text color
    Color_BG : SU.AG_Color;             -- Background color
    Justify  : AG_Text_Justify;         -- Justification mode
    Valign   : AG_Text_Valign;          -- Vertical alignment
    Tab_Wd   : C.int;                   -- Width of tabs in pixels
#if AG_DEBUG
    C_Tag    : AG_Text_State_Tag;
#end if;
    C_Pad1   : Unsigned_32;
  end record
    with Convention => C;
 
  ------------------------------------------
  -- Statically-compiled font description --
  ------------------------------------------
  type AG_Static_Font is array (1 .. $SIZEOF_AG_StaticFont)
    of aliased Unsigned_8 with Convention => C;
  for AG_Static_Font'Size use $SIZEOF_AG_StaticFont * System.Storage_Unit;
  
  ------------------------------
  -- Measure of rendered text --
  ------------------------------
  type AG_Text_Metrics is record
    W,H         : C.int;                -- Dimensions in pixels
    Line_Widths : access C.unsigned;    -- Width of each line
    Line_Count  : C.unsigned;           -- Total line count
    C_Pad1      : Unsigned_32;
  end record
    with Convention => C;
  type Text_Metrics_Access is access all AG_Text_Metrics with Convention => C;
  subtype Text_Metrics_not_null_Access is not null Text_Metrics_Access;

  package Text_Line_Widths_Packages is new Ada.Containers.Indefinite_Vectors
    (Index_Type   => Positive,
     Element_Type => Natural);
  subtype Text_Line_Widths is Text_Line_Widths_Packages.Vector;

  --------------------------
  -- Internal glyph cache --
  --------------------------
  type AG_Glyph_Cache is array (1 .. $SIZEOF_AG_GlyphCache)
    of aliased Unsigned_8 with Convention => C;
  for AG_Glyph_Cache'Size use $SIZEOF_AG_GlyphCache * System.Storage_Unit;

  --
  -- Initialize the font engine.
  --
  function Init_Text_Subsystem return Boolean;

  --
  -- Release all resources allocated by the font engine.
  --
  procedure Destroy_Text_Subsystem;
  
  --
  -- Set the default Agar font (by access to a Font object).
  --
  procedure Set_Default_Font (Font : in Font_not_null_Access)
    with Import, Convention => C, Link_Name => "AG_SetDefaultFont";

  --
  -- Set the default Agar font (by a font specification string).
  --
  -- Syntax: "(family):(size):(style)". Valid field separators include
  -- `:', `,', `.' and `/'. This works with fontconfig if available.
  -- Size is whole points (no fractional allowed with the default font).
  -- Style may include `b' (bold), `i' (italic) and `U' (uppercase).
  --
  procedure Set_Default_Font (Spec : in String);

  --
  -- Load (or fetch from cache) a font.
  --
  function Fetch_Font
    (Family     : in String         := "_agFontVera";
     Size       : in AG_Font_Points := AG_Font_Points(12);
     Bold       : in Boolean        := False;
     Italic     : in Boolean        := False;
     Underlined : in Boolean        := False;
     Uppercase  : in Boolean        := False) return Font_Access;

  --
  -- Decrement the reference count of a font (and free unreferenced fonts).
  --
  procedure Unused_Font (Font : in Font_not_null_Access)
    with Import, Convention => C, Link_Name => "AG_UnusedFont";

  --
  -- Push and pop the font engine rendering state.
  --
  procedure Push_Text_State
    with Import, Convention => C, Link_Name => "AG_PushTextState";
  procedure Pop_Text_State
    with Import, Convention => C, Link_Name => "AG_PopTextState";

  --
  -- Set the current font to the specified family+size+style (or just size).
  --
  procedure Text_Set_Font
    (Family     : in String;
     Size       : in AG_Font_Points := AG_Font_Points(12);
     Bold       : in Boolean := False;
     Italic     : in Boolean := False;
     Underlined : in Boolean := False;
     Uppercase  : in Boolean := False);

  --
  -- Set the current font to a given % of the current font size.
  --
  procedure Text_Set_Font (Percent : in Natural);
  
  --
  -- Set the current font to the referenced font.
  --
  procedure Text_Set_Font (Font : Font_not_null_Access)
    with Import, Convention => C, Link_Name => "AG_TextFont";
  
  --
  -- Return the expected size in pixels of rendered (UTF-8) text.
  --
  procedure Size_Text
    (Text : in     String;
     W,H  :    out Natural);
  procedure Size_Text
    (Text       : in     String;
     W,H        :    out Natural;
     Line_Count :    out Natural);
  procedure Size_Text
    (Text        : in     String;
     W,H         :    out Natural;
     Line_Count  :    out Natural;
     Line_Widths :    out Text_Line_Widths);

  --
  -- Canned dialogs: Display an informational, warning or error message.
  --
  procedure Text_Msg
    (Title : in AG_Text_Message_Title := INFO;
     Text  : in String);
#if AG_TIMERS
  procedure Text_Msg
    (Title : in AG_Text_Message_Title := INFO;
     Text  : in String;
     Time  : in Natural := 2000);
#end if;
  procedure Text_Msg_From_Error
    with Import, Convention => C, Link_Name => "AG_TextMsgFromError";
  procedure Text_Info (Key, Text : in String);
  procedure Text_Warning (Key, Text : in String);
  procedure Text_Error (Text : in String);

  --
  -- Calculate the X,Y offsets required to justify and vertically-align
  -- a text surface of a given size within a given area of pixels.
  --
  procedure Text_Align
    (W_Area, H_Area : in     Natural;
     W_Text, H_Text : in     Natural;
     L_Pad, R_Pad   : in     Natural := 0;
     T_Pad, B_Pad   : in     Natural := 0;
     Justify        : in     AG_Text_Justify := CENTER;
     Valign         : in     AG_Text_Valign := MIDDLE;
     X,Y            :    out Integer);
  function Text_Justify (W_Area, W_Text : in Natural) return Integer;
  function Text_Valign (H_Area, H_Text : in Natural) return Integer;

  --
  -- Render text to a new surface.
  --
  function Text_Render
    (Text : in String) return SU.Surface_not_null_Access;
  function Text_Render
    (Text : in AG_Char_not_null_Access) return SU.Surface_not_null_Access
    with Import, Convention => C, Link_Name => "AG_TextRenderNat";

  --
  -- Render text and blit it to an existing surface.
  --
  procedure Text_Render
    (Text    : in String;                     -- UTF-8
     Surface : in SU.Surface_not_null_Access;
     X,Y     : in Natural := 0);
  procedure Text_Render
    (Text    : in AG_Char_not_null_Access;    -- UCS-4 (internal)
     Surface : in SU.Surface_not_null_Access;
     X,Y     : in Natural := 0);

  --
  -- Lookup (possibly bringing into cache), a glyph.
  --
  function Text_Render_Glyph
    (Driver : in WID.Driver_not_null_Access;
     Char   : in AG_Char) return Glyph_not_null_Access
    with Import, Convention => C, Link_Name => "AG_TextRenderGlyph";

  --
  -- Set State Attribute: Foreground Color.
  --
  procedure Text_Set_Color
    (Color : in SU.Color_not_null_Access)
    with Import, Convention => C, Link_Name => "AG_TextColor";
  procedure Text_Set_Color_8
    (R,G,B : in Unsigned_8;
     A     : in Unsigned_8 := 255)
    with Import, Convention => C, Link_Name => "AG_TextColorRGBA";
  procedure Text_Set_Color_8
    (RGBA : in Unsigned_32)
    with Import, Convention => C, Link_Name => "AG_TextColorHex";

  --
  -- Set State Attribute: Background Color.
  --
  procedure Text_Set_BG_Color
    (Color : in SU.Color_not_null_Access)
    with Import, Convention => C, Link_Name => "AG_TextBGColor";
  procedure Text_Set_BG_Color_8
    (R,G,B : in Unsigned_8;
     A     : in Unsigned_8 := 255)
    with Import, Convention => C, Link_Name => "AG_TextBGColorRGBA";
  procedure Text_Set_BG_Color_8
    (RGBA : in Unsigned_32)
    with Import, Convention => C, Link_Name => "AG_TextBGColorHex";

  --
  -- Set State Attributes: Justification, Alignment, Tab width.
  --
  procedure Text_Set_Justify
    (Justify : AG_Text_Justify)
    with Import, Convention => C, Link_Name => "AG_TextJustify";
  procedure Text_Set_Valign
    (Valign : AG_Text_Valign)
    with Import, Convention => C, Link_Name => "AG_TextValign";
  procedure Text_Set_Tab_Width
    (Width : Natural);

  private

  function AG_InitTextSubsystem return C.int
    with Import, Convention => C, Link_Name => "AG_InitTextSubsystem";

  procedure AG_DestroyTextSubsystem
    with Import, Convention => C, Link_Name => "AG_DestroyTextSubsystem";

  procedure AG_TextParseFontSpec
    (Spec : in CS.chars_ptr)
    with Import, Convention => C, Link_Name => "AG_TextParseFontSpec";

  function AG_FetchFont
    (Family : in CS.chars_ptr;
     Size   : in Font_Points_Access;
     Flags  : in C.unsigned) return Font_Access
    with Import, Convention => C, Link_Name => "AG_FetchFont";

  function AG_TextFontLookup
    (Family : in CS.chars_ptr;
     Size   : in Font_Points_Access;
     Flags  : in C.unsigned) return Font_Access
    with Import, Convention => C, Link_Name => "AG_TextFontLookup";

  function AG_TextFontPct
    (Percent : in C.int) return Font_Access
    with Import, Convention => C, Link_Name => "AG_TextFontPct";

  procedure AG_TextSize
    (Text : in CS.chars_ptr;
     W,H  : access C.int)
    with Import, Convention => C, Link_Name => "AG_TextSize";

  type AG_TextSizeMulti_Line_Entry is array (C.unsigned range <>)
      of aliased C.unsigned with Convention => C;

  package Line_Width_Array is new Interfaces.C.Pointers
    (Index              => C.unsigned,
     Element            => C.unsigned,
     Element_Array      => AG_TextSizeMulti_Line_Entry,
     Default_Terminator => 0);

  procedure AG_TextSizeMulti
    (Text    : in CS.chars_ptr;
     W,H     : access C.int;
     W_Lines : in Line_Width_Array.Pointer;
     N_Lines : access C.unsigned)
    with Import, Convention => C, Link_Name => "AG_TextSizeMulti";

  procedure AG_TextMsgS
    (Title : in AG_Text_Message_Title;
     Text  : in CS.chars_ptr)
    with Import, Convention => C, Link_Name => "AG_TextMsgS";
#if AG_TIMERS
  procedure AG_TextTmsgS
    (Title : in AG_Text_Message_Title;
     Time  : in Unsigned_32;
     Text  : in CS.chars_ptr)
    with Import, Convention => C, Link_Name => "AG_TextTmsgS";
#end if;

  procedure AG_TextInfoS (Key, Text : in CS.chars_ptr)
    with Import, Convention => C, Link_Name => "AG_TextInfoS";

  procedure AG_TextWarningS (Key, Text : in CS.chars_ptr)
    with Import, Convention => C, Link_Name => "AG_TextWarningS";

  procedure AG_TextErrorS (Text : in CS.chars_ptr)
    with Import, Convention => C, Link_Name => "AG_TextErrorS";

  procedure AG_TextAlign
    (X,Y            : access C.int;
     W_Area, H_Area : in C.int;
     W_Text, H_Text : in C.int;
     L_Pad, R_Pad   : in C.int;
     T_Pad, B_Pad   : in C.int;
     Justify        : in AG_Text_Justify;
     Valign         : in AG_Text_Valign)
    with Import, Convention => C, Link_Name => "AG_TextAlign";

  function AG_TextJustifyOffset (W_Area, W_Text : in C.int) return C.int
    with Import, Convention => C, Link_Name => "AG_TextJustifyOffset";

  function AG_TextValignOffset (H_Area, H_Text : in C.int) return C.int
    with Import, Convention => C, Link_Name => "AG_TextValignOffset";

  procedure AG_TextTabWidth (Pixels : in C.int)
    with Import, Convention => C, Link_Name => "AG_TextTabWidth";

  function AG_TextRender (Text : in CS.chars_ptr) return SU.Surface_not_null_Access
    with Import, Convention => C, Link_Name => "AG_TextRender";

end Agar.Text;
