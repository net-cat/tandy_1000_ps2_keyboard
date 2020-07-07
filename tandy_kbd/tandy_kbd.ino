#include <PS2KeyAdvanced.h>

#define PS2_DATA 4
#define PS2_CLOCK 3 // Must be 3 or 2 on Uno/Pro/Pro Mini.

#define TANDY_DATA 5
#define TANDY_CLOCK 6
#define TANDY_BUSY 7

#define LED 13

// Each keypress can insert up to three scancodes in the queue.
// This will keep the buffer from filling up if you hold down a key.
#define MAX_CODES_PER_LOOP 3

//#define SHOW_DEBUG_PRINT

PS2KeyAdvanced g_Keyboard;

void setup()
{
  delay(1000);

  // PS/2 Setup
  g_Keyboard.begin(PS2_DATA, PS2_CLOCK);

  // Tandy Setup
  pinMode(TANDY_DATA, OUTPUT);
  pinMode(TANDY_CLOCK, OUTPUT);
  pinMode(TANDY_BUSY, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(TANDY_DATA, HIGH);
  digitalWrite(TANDY_CLOCK, LOW);
  digitalWrite(LED, LOW);

  // Serial Debug Setup
  Serial.begin(115200);
  Serial.println("Tandy 1000 Keyboard Converter");
}

inline void bit_bang_tandy_symbol(bool value)
{
  digitalWrite(TANDY_DATA, value);
  delayMicroseconds(5);
  digitalWrite(TANDY_CLOCK, HIGH);
  delayMicroseconds(12);
  digitalWrite(TANDY_DATA, HIGH);
  delayMicroseconds(14);
  digitalWrite(TANDY_CLOCK, LOW);
}

class ScopedNoInterrupts
{
  public:
    ScopedNoInterrupts()
    {
      noInterrupts();
    }

    ~ScopedNoInterrupts()
    {
      interrupts();
    }
};

inline void bit_bang_tandy_stop_bit()
{
  digitalWrite(TANDY_DATA, LOW);
  delayMicroseconds(5);
  digitalWrite(TANDY_DATA, HIGH);
}

void bit_bang_tandy_scancode(uint8_t code)
{
  if(!code)
  {
    return;
  }

  #ifdef SHOW_DEBUG_PRINT
  Serial.print("Sending Tandy Scancode: ");
  Serial.println(code, HEX);
  #endif

  char pre_computed[8];

  for (uint8_t i = 0; i < 8; ++i)
  {
    pre_computed[i] = ((code >> i) & 0x01);
  }

  noInterrupts();
  bit_bang_tandy_symbol(pre_computed[0]);
  bit_bang_tandy_symbol(pre_computed[1]);
  bit_bang_tandy_symbol(pre_computed[2]);
  bit_bang_tandy_symbol(pre_computed[3]);
  bit_bang_tandy_symbol(pre_computed[4]);
  bit_bang_tandy_symbol(pre_computed[5]);
  bit_bang_tandy_symbol(pre_computed[6]);
  bit_bang_tandy_symbol(pre_computed[7]);
  bit_bang_tandy_stop_bit();
  interrupts();
}

/*
Map from PS2KeyAdvanced scan codes to Tandy Scan Codes

The high bit meaning depends on if the code is affected_by_num_lock or not.

If yes, Tandy Num Lock must be ON if the high bit is set and OFF if the low bit is not set.
If no, Tandy Shift must be pressed if the high bit is set.

Codes 0x80 and up are things like ACPI and Media buttons, so we're ignoring them.
*/
const uint8_t PS2_TO_TANDY_NORMAL_LOOKUP[128] = {
  /* 0x00 */ 0x00, 0x45, 0x00, 0x3a, 0x00, 0x00, 0x2a, 0x36, /* 0x07 */
  /* 0x08 */ 0x1d, 0x1d, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, /* 0x0f */
  /* 0x10 */ 0x00, 0x58, 0x4f, 0x49, 0x51, 0x2b, 0x4e, 0x29, /* 0x17 */
  /* 0x18 */ 0x4a, 0x55, 0x53, 0x01, 0x0e, 0x0f, 0x1c, 0x39, /* 0x1f */
  /* 0x20 */ 0x52, 0x51, 0x50, 0x4f, 0x4d, 0x4c, 0x4b, 0x47, /* 0x27 */
  /* 0x28 */ 0x48, 0x49, 0x56, 0x57, 0xd5, 0xd3, 0x89, 0x35, /* 0x2f */
  /* 0x30 */ 0x0b, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, /* 0x37 */
  /* 0x38 */ 0x09, 0x0a, 0x28, 0x33, 0x0c, 0x34, 0x35, 0x00, /* 0x3f */
  /* 0x40 */ 0x50, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, /* 0x47 */
  /* 0x48 */ 0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, /* 0x4f */
  /* 0x50 */ 0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, /* 0x57 */
  /* 0x58 */ 0x2d, 0x15, 0x2c, 0x27, 0x47, 0x1a, 0x1b, 0x0d, /* 0x5f */
  /* 0x60 */ 0x00, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, /* 0x67 */
  /* 0x68 */ 0x42, 0x43, 0x44, 0x59, 0x5a, 0x00, 0x00, 0x00, /* 0x6f */
  /* 0x70 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x77 */
  /* 0x78 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x7f */
};

/* This is a special lookup table for when the PS/2 Num Lock is on.
 *
 * It only has entries for 0x20-0x2f because only the numpad is affected
 * by Num Lock. When num lock is off, PS2_TO_TANDY_NORMAL_LOOKUP is used.
 *
 * All other codes will always use PS2_TO_TANDY_NORMAL_LOOKUP.
 *
 * The high bit has the same meaning as in PS2_TO_TANDY_NORMAL_LOOKUP.
 */
const uint8_t PS2_TO_TANDY_NUMLOCK_LOOKUP[16] = {
  /* 0x20 */ 0xd2, 0xcf, 0xd0, 0xd1, 0xcb, 0xcc, 0xcd, 0xc7, /* 0x27 */
  /* 0x28 */ 0xc8, 0xc9, 0x56, 0x57, 0xd5, 0xd3, 0x89, 0x35, /* 0x2f */
};

/* This function is for PS/2 keys whose shift actions don't match up with
 * the shift action of their Tandy counterparts.
 *
 * This could also be a lookup table, but there are only two keys so it seemed
 * like a waste.
 *
 * The high bit in the return has the same meaning as in PS2_TO_TANDY_NORMAL_LOOKUP.
 */
inline uint8_t special_shift_action(uint8_t ps2_code)
{
  switch(ps2_code)
  {
    case 0x40: // Grave (`) / Tilde (~)
      return 0xc8; // (code 0x48 with shift)
    case 0x5c: // Backslash (\) / Pipe (|)
      return 0xcb; // (code 0x4b with shift)
  }
  return 0;
}

inline uint8_t xt_compat_action(uint8_t ps2_code)
{
  switch(ps2_code)
  {
    case 0x15: // left arrow
      return 0x4b;
    case 0x16: // right arrow
      return 0x4d;
    case 0x17: // up arrow
      return 0x48;
    case 0x18: // down arrow
      return 0x50;
    case 0x6b: // F11
      return 0x57;
    case 0x6c: // F12
      return 0x58;
  }
  return 0;
}

// Defines for some of the Tandy modifier keys. (Just to make the code easier to read.)
#define TANDY_CTRL 0x1du
#define TANDY_LSHIFT 0x2au
#define TANDY_RSHIFT 0x36u
#define TANDY_ALT 0x38u
#define TANDY_CAPSLOCK 0x3au
#define TANDY_NUMLOCK 0x45u

// This union makes is more readable to decode PS2KeyAdvanced codes.
union PS2KeyAdvancedCode
{
  uint16_t raw; // raw must always be the first element in the union
  struct
  {
    uint8_t code;
    union
    {
      uint8_t status_bits;
      struct
      {
        unsigned FUNCTION : 1;
        unsigned GUI : 1;
        unsigned ALT_GR : 1;
        unsigned ALT : 1;
        unsigned CAPS : 1;
        unsigned CTRL : 1;
        unsigned SHIFT : 1;
        unsigned BREAK : 1;
      } __attribute__((packed));
    } __attribute__((packed));
  } __attribute__((packed));
} __attribute__((packed));

// These keep track of the *Tandy* state, not the PS/2 state.
bool g_LShiftPressed = false;
bool g_RShiftPressed = false;
bool g_CtrlPressed = false;
bool g_AltPressed = false;
bool g_CapsLockActive = false;
bool g_NumLockActive = false;
bool g_XTCompatMode = false;

// Tandy Scan Code Buffer
uint8_t g_TandyBuffer[256]; // Yes, this is so I can use uint8_t wrapping.
uint8_t g_TandyBufferFront = 0;
uint8_t g_TandyBufferBack = 0;

inline bool has_tandy_scancode()
{
  return g_TandyBufferFront != g_TandyBufferBack;
}

void enqueue_tandy_scancode(uint8_t code)
{
  g_TandyBuffer[g_TandyBufferBack++] = code;
}

uint8_t dequeue_tandy_scancode()
{
  if (has_tandy_scancode())
  {
    return g_TandyBuffer[g_TandyBufferFront++];
  }
  return 0u;
}

// These Tandy scan codes are affected by caps lock.
// (Not currently used since they are the same on Tandy and PS/2)
bool affected_by_caps_lock(uint8_t tandy_code)
{
  return
    (tandy_code >= 0x10 && tandy_code <= 0x19) ||
    (tandy_code >= 0x1e && tandy_code <= 0x26) ||
    (tandy_code >= 0x2c && tandy_code <= 0x32);
}

// These Tandy scan codes are affected by num lock.
bool affected_by_num_lock(uint8_t tandy_code)
{
  return
    (tandy_code >= 0x47 && tandy_code <= 0x49) ||
    (tandy_code >= 0x4b && tandy_code <= 0x4d) ||
    (tandy_code >= 0x4f && tandy_code <= 0x53) ||
    (tandy_code >= 0x55 && tandy_code <= 0x57);
}

bool set_flag_detect_change(bool& flag, bool new_value)
{
  if(flag != new_value)
  {
    flag = new_value;
    return true;
  }
  return false;
}

/* Decode the code from the lookup table and send an appropriate combination of
 * scan codes to get the desired action.
 *
 * tandy_code is the result from the lookup tables, NOT a raw Tandy scan code.
 * key_break is true if this is a break event, false if this is a make event.
 */
void transmit_tandy_code(uint8_t tandy_code, bool key_break)
{
  if(!tandy_code)
  {
    #ifdef SHOW_DEBUG_PRINT
    Serial.println("*** DEAD KEY ***");
    #endif
    return;
  }

  // Handle modifier keys.
  switch(tandy_code)
  {
    case TANDY_CTRL:
      if (!set_flag_detect_change(g_CtrlPressed, !key_break))
      {
        return;
      }
      break;
    case TANDY_LSHIFT:
      if (!set_flag_detect_change(g_LShiftPressed, !key_break))
      {
        return;
      }
      break;
    case TANDY_RSHIFT:
      if (!set_flag_detect_change(g_RShiftPressed, !key_break))
      {
        return;
      }
      break;
    case TANDY_ALT:
      if (!set_flag_detect_change(g_AltPressed, !key_break))
      {
        return;
      }
      break;
    case TANDY_CAPSLOCK:
      if (set_flag_detect_change(g_CapsLockActive, g_Keyboard.getLock() & PS2_LOCK_CAPS))
      {
        if (g_CapsLockActive)
        {
          ScopedNoInterrupts sni;
          enqueue_tandy_scancode(TANDY_CAPSLOCK);
        }
        else
        {
          ScopedNoInterrupts sni;
          enqueue_tandy_scancode(TANDY_CAPSLOCK | 0x80u);
        }
        delay(100); // I'm not sure why, but the sketch freezes without this.
      }
      return;
    case TANDY_NUMLOCK:
      if (set_flag_detect_change(g_NumLockActive, g_Keyboard.getLock() & PS2_LOCK_NUM))
      {
        if (g_NumLockActive)
        {
          ScopedNoInterrupts sni;
          enqueue_tandy_scancode(TANDY_NUMLOCK);
        }
        else
        {
          ScopedNoInterrupts sni;
          enqueue_tandy_scancode(TANDY_NUMLOCK | 0x80u);
        }
        delay(100); // I'm not sure why, but the sketch freezes without this.
      }
      return;
  }

  // Since PS/2 codes don't map perfectly into Tandy codes, we may have to send some modifiers.
  bool needs_modifier = tandy_code & 0x80u;
  bool shift = false;
  bool numlock = false;
  bool unnumlock = false;
  tandy_code &= 0x7fu;

  if(affected_by_num_lock(tandy_code))
  {
    numlock = needs_modifier && !g_NumLockActive;
    unnumlock = !needs_modifier && g_NumLockActive;
  }
  else
  {
    shift = needs_modifier && !(g_LShiftPressed || g_RShiftPressed);
  }

  if(key_break)
  {
    ScopedNoInterrupts sni;

    enqueue_tandy_scancode(tandy_code | 0x80u);
    if(shift)
    {
      enqueue_tandy_scancode(TANDY_RSHIFT | 0x80u);
    }
    if(numlock)
    {
      enqueue_tandy_scancode(TANDY_NUMLOCK | 0x80u);
    }
    if(unnumlock)
    {
      enqueue_tandy_scancode(TANDY_NUMLOCK);
    }

    // Flash LED
    digitalWrite(LED, HIGH);
  }
  else
  {
    ScopedNoInterrupts sni;

    if(shift)
    {
      enqueue_tandy_scancode(TANDY_RSHIFT);
    }
    if(numlock)
    {
      enqueue_tandy_scancode(TANDY_NUMLOCK);
    }
    if(unnumlock)
    {
      enqueue_tandy_scancode(TANDY_NUMLOCK | 0x80u);
    }
    enqueue_tandy_scancode(tandy_code);

    // Unflash LED
    digitalWrite(LED, LOW);
  }
}

uint8_t convert_ps2_to_tandy_code(union PS2KeyAdvancedCode ps2_code)
{
  uint8_t tandy_code = 0;
  if(ps2_code.code < 128)
  {
    // Look up the num lock code, if applicable.
    if(g_NumLockActive && ps2_code.code >= 0x20 && ps2_code.code < 0x30)
    {
      tandy_code = PS2_TO_TANDY_NUMLOCK_LOOKUP[ps2_code.code - 0x20];
    }

    // Check for XT Compatibility Mode
    else if(g_XTCompatMode)
    {
      tandy_code = xt_compat_action(ps2_code.code);
    }

    // Look up the shift code, if applicable.
    else if(ps2_code.SHIFT)
    {
      tandy_code = special_shift_action(ps2_code.code);
    }

    // If not shifted or no special code for shift, look up normal code.
    if(!tandy_code)
    {
      tandy_code = PS2_TO_TANDY_NORMAL_LOOKUP[ps2_code.code];
    }
  }
  return tandy_code;
}

void loop()
{
  PS2KeyAdvancedCode ps2_code = {0};

  if (g_Keyboard.available())
  {
    // read the next key
    ps2_code = {g_Keyboard.read()};
  }

  if (digitalRead(TANDY_BUSY))
  {
    if(ps2_code.raw > 0)
    {
      if(ps2_code.code == 0x2)
      {
        g_XTCompatMode = g_Keyboard.getLock() & PS2_LOCK_SCROLL;
      }
      else
      {
        uint8_t tandy_code = convert_ps2_to_tandy_code(ps2_code);
        transmit_tandy_code(tandy_code, ps2_code.BREAK);
      }

      #ifdef SHOW_DEBUG_PRINT
      Serial.print( "Value " );
      Serial.print( ps2_code.raw, HEX );
      Serial.print( " - Status Bits " );
      Serial.print( ps2_code.status_bits, HEX );
      Serial.print( "  Code " );
      Serial.println( ps2_code.code, HEX );
      #endif
    }

    for(uint8_t i = 0; i < MAX_CODES_PER_LOOP; ++i)
    {
      uint8_t tandy_code = 0;
      {
        ScopedNoInterrupts sni;
        if (!has_tandy_scancode())
        {
          break;
        }
        tandy_code = dequeue_tandy_scancode();
      }
      bit_bang_tandy_scancode(tandy_code);
    }
  }
}
