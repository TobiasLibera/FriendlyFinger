/*
 * Code for the FriendlyFinger project.
 * Written by Tobias Libera and published under the alias "SarcasticEngineering".
 * 
 * All data for this project can be found here:
 * https://github.com/TobiasLibera/FriendlyFinger
 * 
 * All data of this project is licensed under the
 * GNU General Public License 3
 * 
 * 2022-12-03
 */
 
#include <WiFi.h>
#include <Arduino.h>

#include <analogWrite.h>



/*
 * Pin Definitions
 */

#define STEPPER_STP         23
#define STEPPER_DIR         22
#define STEPPER_EN          32

#define RGB_L_RED           21
#define RGB_L_GRN           19
#define RGB_L_BLU           18

#define RGB_R_RED           17
#define RGB_R_GRN           16
#define RGB_R_BLU           4



/*
 * WiFi Definitions
 */

IPAddress LOCAL_IP          (192, 168, 1,   2) ;
IPAddress GATEWAY           (192, 168, 1,   1) ;
IPAddress SUBNET            (255, 255, 255, 0) ;
IPAddress DNS               (192, 168, 1,   1) ;

#define WIFI_NAME           "your_funny_wifi_name"
#define WIFI_PASSWORD       "your_secure_wifi_password_123456"

#define WIFI_RECONNECT_TIMEOUT  5000                            /* Milliseconds of not available Wifi, until reconnect try */



/*
 * Controls Definitions
 */
                                              /* ATTENTION: Only have one of the Control Definitions uncommented! */
#define WEBSERVER                             /* If defined: The ESP32 will host a webserver for turning the Friendly Finger on and off *//
//#define UDP_SERVER                          /* If defined: Enables the receiving of UDP messages for turning the Friendly Finger on and off */

#define _SERVER_PORT                    80    /* Port where Webserver will be hosted OR UDP-server is listening */

#define WEBSERVER_BACKGROUND_COLOR      String("#A9A9A9")   /* Color definitions for the webserver */
#define WEBSERVER_TITLE_COLOR           String("#000000")
#define WEBSERVER_BUTTON_ON_COLOR       String("#008B8B")       
#define WEBSERVER_BUTTON_OFF_COLOR      String("#556B2F")
#define WEBSERVER_BUTTON_TEXT_COLOR     String("#FFFFFF")

#define UDP_FF_ON                       42    /* Message to receive to enable the functions of the Friendly Finger */
#define UDP_FF_OFF                      69    /* Message to receive to disable the functions of the Friendly Finger */



/*
 * Movement Definitions
 */

#define MOVEMENT_ANGLE                130       /* Movement angle from complete to the left to complete to the right */
#define MOVEMENT_SPEED                8         /* Duration in seconds for one movement from left to right */
#define STEPS_PER_ROTATION            6400      /* Number of steps for one rotation of the stepper motor (Microstepping) */
#define TIME_AT_DIRECTION_REVERSE     300       /* Number of milliseconds to wait at movment extreme point before direction is reversed */



/*
 *  Lightshow Definitions
 */

#define BLINK_DOUBLE_ON               300       /* Duration in milliseconds the light stays on */
#define BLINK_DOUBLE_OFF              750       /* Duration in milliseconds the light stays off */
#define BLINK_DOUBLE_CYCLES           4         /* Number of cycles the blink double programm is running through */

#define RAINBOW_CYCLE_TIME            20        /* Duration in seconds from starting a color till the color is reached again */
#define RAINBOW_CYCLES                2         /* Number of cycles the rainbow programm is running through */
 
#define BLINK_SINGLE_ON               300       /* Duration in milliseconds the light stays on */
#define BLINK_SINGLE_OFF              300       /* Duration in milliseconds the light stays off */
#define BLINK_SINGLE_CYCLES           5         /* Number of cycles the blink double programm is running through */

#define RAINBOW_SPLITTED_SWITCH_TIME  250       /* Time afted LED is switched */
#define RAINBOW_SPLITTED_CYCLE_TIME   13        /* Duration in seconds from starting a color till the color is reached again */
#define RAINBOW_SPLITTED_CYCLES       4         /* Number of cycles the rainbow programm is running through */

#define WHITE_BLINK_TIMEOUT_BEGIN     500       /* On-Off time in milliseconds where white blink starts */
#define WHITE_BLINK_TIMEOUT_DECRMNT   30        /* On-Off time decrement in milliseconds per blink */
#define WHITE_BLINK_TIMEOUT_FADE      4         /* Fade timeout in milliseconds for each change in brightness */

#define LIGHTSHOW_ALL_OVER_CYCLES     3         /* Number of cycles to walk through blink and rainbow program till while blink cycle end */




#ifdef WEBSERVER
  #include <WebServer.h>
  WebServer server ( _SERVER_PORT ) ;
#endif

#ifdef UDP_SERVER
  #include <WiFiUdp.h>
  WiFiUDP server ;
#endif




bool DO_FUNCTIONS = false ;




#ifdef WEBSERVER
  class WebServerManager
  {
  
    public:
  
      WebServerManager (WebServer* server)
      {
        /* 
         * Constructor for Class WebServerManager 
         * Takes a pointer to Webserver Class as argument
         */
        this -> web_server = server ;
      }
    
    
      void init(uint16_t dummy_port)
      {
        /*
         * Defines the subdomains
         * Inits the webserver
         */
        this -> web_server -> on("/"      , this->handle_connect) ;
        this -> web_server -> on("/ff_on" , this->handle_ff_on) ;
        this -> web_server -> on("/ff_off", this->handle_ff_off) ;
        
        this -> web_server -> onNotFound(this->handle_not_found) ;
    
        this -> web_server -> begin() ;
      }
    
    
      void work()
      {
        /*
         * This function will be called from main loop() function
         * Handles client requests to the webserver
         */
        this -> web_server -> handleClient() ;
      }
  
  
    private:
  
      static void handle_connect()
      {
        /*
         * Handles subdomain "/"
         */
        server.send(200, "text/html", send_HTML(DO_FUNCTIONS)) ; 
      }
    
    
      static void handle_ff_on()
      {
        /*
         * Handles subdomain "/ff_on"
         * Enables the functions of the Friendly Finger
         */
        DO_FUNCTIONS = true ;
        server.send(200, "text/html", send_HTML(DO_FUNCTIONS)) ; 
      }
    
    
      static void handle_ff_off()
      {
        /*
         * Handles subdomain "/ff_off"
         * Disables the functions of the Friendly Finger
         */
        DO_FUNCTIONS = false ;
        server.send(200, "text/html", send_HTML(DO_FUNCTIONS)) ; 
      }
    
    
      static void handle_not_found()
      {
        /*
         * Handling of all subdomains, which are not defined
         */
        server.send(404, "text/plain", "Not found") ;
      }
    
    
      static String send_HTML (bool is_functions)
      {
        /*
         * HTML script, which is send to the client
         */
        String ptr = "<!DOCTYPE html> <html>\n" ;
          ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n" ;
          ptr +="<title>Friendly Finger Control</title>\n" ;
          ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n" ;
          ptr +="body{margin-top: 50px;background-color:"+WEBSERVER_BACKGROUND_COLOR+"} h1 {color: "+WEBSERVER_TITLE_COLOR+";margin: 50px auto 30px;}\n" ;
          ptr +=".button {display: block;width: 80px;;border: none;color: "+WEBSERVER_BUTTON_TEXT_COLOR+";padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n" ;
          ptr +=".button-on {background-color: "+WEBSERVER_BUTTON_ON_COLOR+";}\n" ;
          ptr +=".button-off {background-color: "+WEBSERVER_BUTTON_OFF_COLOR+";}\n" ;
          ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n" ;
          ptr +="</style>\n" ;
          ptr +="</head>\n" ;
          ptr +="<body>\n" ;
          ptr +="<h1>Friendly Finger</h1>\n" ;
          
          if (is_functions)
          {ptr +="<a class=\"button button-off\" href=\"/ff_off\">OFF</a>\n" ;}
          else
          {ptr +="<a class=\"button button-on\" href=\"/ff_on\">ON</a>\n" ;}
    
          ptr +="</body>\n" ;
          ptr +="</html>\n" ;
          return ptr ;
      }
    
    
      WebServer* web_server ;
    
  } ;
#endif




#ifdef UDP_SERVER
  class UdpServerManager
  {
    
    public:
  
      UdpServerManager (WiFiUDP* server)
      {
        /* 
         * Constructor for Class UdpServerManager 
         * Takes a pointer to WiFiUDP Class as argument
         */
        this -> udp_server = server ;
      }


      void init(uint16_t port)
      {
        /*
         * Inits the UDP Server
         * Takes the port, on which the udp server is listening, as argument
         */
        this -> udp_server -> begin(port) ;
      }


      void work()
      {
        /*
         * This function will be called from main loop() function
         * Handles incoming UDP messages, checks if they correspond to the defined
         * Commands of enabling and disabling the functions of the Friendly Finger
         */
        int32_t packetSize = this -> udp_server -> parsePacket() ;
        if (packetSize)
        {
          for (uint8_t i = 0; i < 255; i += 1)
          {
            this -> packet[i] = 0 ;
          }
          
          this -> udp_server -> read(this->packet, 255) ;
          this -> udp_server -> flush() ;

          int16_t udp_input = atoi(this->packet) ;

          if (udp_input > 0)
          {
            if (udp_input == UDP_FF_ON)
            {
              DO_FUNCTIONS = true ;
            }
            else if (udp_input == UDP_FF_OFF)
            {
              DO_FUNCTIONS = false ;
            }
          }
        }
      }


    private:

      char packet[255] ;

      WiFiUDP* udp_server ;
      
  } ;
#endif




class WifiManager
{

  public:

    void init()
    {
      /*
       * Configuration of the WiFi settings of the ESP32 
       * Mainly sets the IP, which the ESP32 can be accessed at
       */
       WiFi.config(LOCAL_IP, GATEWAY, SUBNET, DNS) ;
    }

    void check()
    {
      /*
         * This function will be called from main loop() function
         * Handles the connection to the local network
         * Trys to reconnect to the local network if connection is broken
         */
      if (WiFi.status() != WL_CONNECTED)
      {
        if (millis() >= this->reconnect_timer + WIFI_RECONNECT_TIMEOUT)
        {
          WiFi.disconnect() ;
          WiFi.begin(WIFI_NAME, WIFI_PASSWORD) ;
          this->reconnect_timer = millis() ;
        }
        
        else if (this->reconnect_timer == 0)
        {
          WiFi.begin(WIFI_NAME, WIFI_PASSWORD) ;
          this->reconnect_timer = millis() ;
        }    
      }
    }


  private:

    uint64_t reconnect_timer = 0 ;
    
} ;




class Stepper
{

  public:

    Stepper (uint8_t stp_pin, uint8_t dir_pin, uint8_t en_pin)
    {
      /*
       * Constructor for Class for Stepper Motor
       * Defines Step Pin, Direction Pin and Enable Pin of the DRV8825 Stepper Motor Driver
       * Defines this pins as output and makes sure, the voltage level of the pins
       * are indeed set to 0V or 1V (Enable)
       * Calculates Parameters needed for the movement from global definitions
       * Sets current position to center position as thats where the movement starts
       */
      this -> stp_pin = stp_pin ;
      this -> dir_pin = dir_pin ;
      this -> en_pin  = en_pin ;

      pinMode(this->stp_pin, OUTPUT) ;
      pinMode(this->dir_pin, OUTPUT) ;
      pinMode(this->en_pin, OUTPUT) ;

      digitalWrite(this->stp_pin, 0) ;
      digitalWrite(this->dir_pin, 0) ;
      digitalWrite(this->en_pin, 1) ;

      this -> calc_parameters() ;

      this -> current_steps = this -> steps_per_half_move ;
    }


    void work()
    {
      /*
       * This function will be called from main loop() function and works with timers
       * Manages the movemet of the stepper motor
       */
      if (DO_FUNCTIONS || do_final_move)
      {
        /*
         * If functions of the Friendly Finger are enabled OR functions were shut off and Finger is moving to center position
         */
        digitalWrite(this->en_pin, 0) ;
         
        if (DO_FUNCTIONS && do_final_move)
        {
          /*
           * If functions of Friendly Finger are enabled while it's also moved to center position for shut down,
           * the mode for the final move is disabled
           */
          do_final_move = false ;
        }

        if (this->wait_for_reverse)
        {
          /*
           * If waiting for reverse direction, do nothing but waiting till timer is done
           */
          if (millis() >= this->wait_reverse_time + TIME_AT_DIRECTION_REVERSE)
          {
            this -> wait_for_reverse = false ;
          }
        }
        else
        {
          /*
           * If not waiting for reverse, wait for step timer and do one single step
           */
          if (millis() > this->last_step_time + this->millis_per_step)
          {
            this -> do_step() ;
            this -> last_step_time = millis() ;
          }
        }

        if (this->current_steps >= steps_per_full_move)
        {
          /*
           * Reverse direction, zero, steps and init the waiting for reverse  
           */
          this -> set_dir(!this->current_dir) ;
          this -> zero_steps() ;

          this -> wait_for_reverse = true ;
          this -> wait_reverse_time = millis() ;
        }
      }
      
      if (!DO_FUNCTIONS)
      {
        /*
         * If functions of the Friendly Finger is shut down
         */
        if (this-> current_steps != steps_per_half_move)
        {
          /*
           * Enables the movement to center position if current position is not center position
           */
          this -> do_final_move = true ;
        }
        else
        {
          /*
           * If current position is center position
           */
          if (this->do_final_move)
          {
            /*
             * Disables movement to center position and sets current direction to initial value
             */
            this -> do_final_move = false ;
          }
          this -> set_dir(0) ;
          digitalWrite(this->en_pin, 1) ;
        }
      }
    }


  private:

    void do_step()
    {
      /*
       * Does one single step
       */
      digitalWrite(this->stp_pin, 1) ;
      digitalWrite(this->stp_pin, 0) ;
      this -> current_steps += 1 ;
    }


    void zero_steps()
    {
      /*
       * Sets the current steps to zero
       */
      this -> current_steps = 0 ;
    }


    void set_dir(bool dir)
    {
      /*
       * Sets direction of stepper motor
       */
      digitalWrite(this->dir_pin, dir) ;
      this -> current_dir = dir ;
    }


    void calc_parameters()
    {
      /*
       * Calculates parameters for the movemet from global parameter definitions
       */
      this -> steps_per_full_move = (uint32_t)( (float)STEPS_PER_ROTATION * ( (float)MOVEMENT_ANGLE / 360.0 ) ) ;

      this -> steps_per_half_move = this->steps_per_full_move / 2 ;
      
      this -> millis_per_step = (uint64_t)( (MOVEMENT_SPEED * 1000) / this->steps_per_full_move ) ;
    }

    uint8_t   stp_pin ;
    uint8_t   dir_pin ;
    uint8_t   en_pin ;
    
    uint32_t  current_steps = 0 ;
    bool      current_dir   = 0 ;

    uint64_t  millis_per_step ;
    uint64_t  last_step_time = 0 ;
    
    uint32_t  steps_per_full_move ;
    uint32_t  steps_per_half_move ;

    bool      wait_for_reverse  = false ;
    uint64_t  wait_reverse_time = 0 ;

    bool      do_final_move = false ;

} ;




class RgbLed
{

  public:

    RgbLed (uint8_t red, uint8_t green, uint8_t blue)
    {
      /*
       * Constructor for Class for RGB-LED
       * Defines Pins for the red, green and blue anode of the RGB-LED
       * Defines this pins as output and makes sure, the voltage level of the pins
       * are indeed set to 0V
       */
      this -> r = red ;
      this -> g = green ;
      this -> b = blue ;

      pinMode(this->r, OUTPUT) ;
      pinMode(this->g, OUTPUT) ;
      pinMode(this->b, OUTPUT) ;

      digitalWrite(this->r, 0) ;
      digitalWrite(this->g, 0) ;
      digitalWrite(this->b, 0) ;
    }


    void write( uint8_t r_pwm, uint8_t g_pwm, uint8_t b_pwm)
    {
      /*
       * Function for writing PWM values to the 3 anodes of the RGB-LED.
       * Takes values from 0 - 255 which maps to 0 - 3.3V
       */
      analogWrite(this->r, r_pwm) ;
      analogWrite(this->g, g_pwm) ;
      analogWrite(this->b, b_pwm) ;
    }


    void off()
    {
      /*
       * Turns off the RGB-LED
       */
      analogWrite(this->r, 0) ;
      analogWrite(this->g, 0) ;
      analogWrite(this->b, 0) ;
    }


  private:

    uint8_t r ;
    uint8_t g ;
    uint8_t b ;

} ;




class Lightshow
{
  /*
   * Feel free to develop your own Lightshows!
   */

  public:

    Lightshow (RgbLed* left_led, RgbLed* right_led)
    {
      /*
       * Constructor for Class for the Lightshow
       * Inherits 2 pointers to the left and to the right RGB-LED of the Friendly Finger
       * Calculates parameters from global definitions
       */
      this -> left = left_led ;
      this -> right = right_led ;

      this -> calc_parameters() ;
    }


    void work()
    {
      /*
       * This function will be called from main loop() function and works with timers
       * Manages the lightshow modes if the DO_FUNCTIONS variable is set to true
       * Shuts off the RGB-LEDS if the DO_FUNCTIONS variable is set to false
       */
      if (DO_FUNCTIONS)
      {
        this -> is_resetted = false ;
        
        if (this->lightshow_mode_ident == 0)
        {
          this -> blink_double() ;
        }
        else if (this->lightshow_mode_ident == 1)
        {
          this -> rainbow() ;
        }
        else if (this->lightshow_mode_ident == 2)
        {
          this -> blink_single() ;
        }
        else if (this->lightshow_mode_ident == 3)
        {
          this -> rainbow_splitted() ;
        }     
        else if (this->lightshow_mode_ident == 42)
        {
          this -> white_blink_cycle_end() ;
        }
      }
      else
      {
        /*
         * If functions of the Friendly Finger are disabled, 
         * the LEDs are turned off the the lightshow is reset to 
         * initial settings
         */
        this -> all_off() ;
        this -> reset() ;
      }
    }


  private:

    void blink_double()
    {
      /*
       * Both LEDs blink simultaneously with changing colors
       */
      if (!this->blink_double_is_on && millis() >= this->blink_double_timer + BLINK_DOUBLE_OFF)
      {
        if (this->blink_double_mode_ident == 0)
        {
          this -> left  -> write(255, 0, 0) ;
          this -> right -> write(255, 0, 0) ;
        }
        else if (this->blink_double_mode_ident == 1)
        {
          this -> left  -> write(255, 60, 0) ;
          this -> right -> write(255, 60, 0) ;
        }
        else if (this->blink_double_mode_ident == 1)
        {
          this -> left  -> write(255, 200, 0) ;
          this -> right -> write(255, 200, 0) ;
        }
        else if (this->blink_double_mode_ident == 2)
        {
          this -> left  -> write(100, 220, 0) ;
          this -> right -> write(100, 220, 0) ;
        }
        else if (this->blink_double_mode_ident == 3)
        {
          this -> left  -> write(255, 0, 255) ;
          this -> right -> write(255, 0, 255) ;
        }
        else if (this->blink_double_mode_ident == 4)
        {
          this -> left  -> write(0, 0, 255) ;
          this -> right -> write(0, 0, 255) ;
        }
        else if (this->blink_double_mode_ident == 5)
        {
          this -> left  -> write(0, 255, 0) ;
          this -> right -> write(0, 255, 0) ;
        }
        else if (this->blink_double_mode_ident == 6)
        {
          this -> left  -> write(0, 255, 40) ;
          this -> right -> write(0, 255, 40) ;
        }
        else if (this->blink_double_mode_ident == 7)
        {
          this -> left  -> write(0, 255, 255) ;
          this -> right -> write(0, 255, 255) ;
        }
        else if (this->blink_double_mode_ident == 8)
        {
          this -> left  -> write(42, 255, 0) ;
          this -> right -> write(42, 255, 0) ;
        }
        else if (this->blink_double_mode_ident == 9)
        {
          this -> left  -> write(120, 0, 255) ;
          this -> right -> write(120, 0, 255) ;
        }
        else if (this->blink_double_mode_ident == 10)
        {
          this -> left  -> write(0, 120, 160) ;
          this -> right -> write(0, 120, 160) ;
        }
        else if (this->blink_double_mode_ident == 11)
        {
          this -> left  -> write(42, 180, 32) ;
          this -> right -> write(42, 180, 32) ;
        }

        this->blink_double_timer = millis() ;
        this->blink_double_is_on = true ;
      }
      
      else if (this->blink_double_is_on && millis() >= this->blink_double_timer + BLINK_DOUBLE_ON)
      {
        this -> all_off() ;

        this->blink_double_is_on = false ;

        this->blink_double_mode_ident += 1 ;

        this->blink_double_timer = millis() ;

        if (this->blink_double_mode_ident == 12)
        {
          this->blink_double_mode_ident = 0 ;
          this->blink_double_cycle_ident += 1 ;

          if (this->blink_double_cycle_ident == BLINK_DOUBLE_CYCLES)
          {
            this -> blink_double_cycle_ident = 0 ;
            this -> lightshow_mode_ident += 1 ;
            this -> all_off() ;
          }
        }
      }
    }


    void rainbow()
    {
      /*
       * Both LEDs walk through the whole Hue spectrum simultaneously
       */
      if (millis() >= this->rainbow_timer + this->rainbow_timeout)
      {
        this -> left  -> write(rainbow_pwm_1, rainbow_pwm_2, rainbow_pwm_3) ;
        this -> right -> write(rainbow_pwm_1, rainbow_pwm_2, rainbow_pwm_3) ;

        if (this->rainbow_pwm_1 == 255 && this->rainbow_pwm_2 < 255 && this->rainbow_pwm_3 == 0)
        {
          this -> rainbow_pwm_2 += 1 ;
        }
        else if (this->rainbow_pwm_1 > 0 && this->rainbow_pwm_2 == 255 && this->rainbow_pwm_3 == 0)
        {
          this -> rainbow_pwm_1 -= 1 ;
        }
        else if (this->rainbow_pwm_1 == 0 && this->rainbow_pwm_2 == 255 && this->rainbow_pwm_3 < 255)
        {
          this -> rainbow_pwm_3 += 1 ;
        }
        else if (this->rainbow_pwm_1 == 0 && this->rainbow_pwm_2 > 0 && this->rainbow_pwm_3 == 255)
        {
          this -> rainbow_pwm_2 -= 1 ;
        }
        else if (this->rainbow_pwm_1 < 255 && this->rainbow_pwm_2 == 0 && this->rainbow_pwm_3 == 255)
        {
          this -> rainbow_pwm_1 += 1 ;
        }
        else if (this->rainbow_pwm_1 == 255 && this->rainbow_pwm_2 == 0 && this->rainbow_pwm_3 > 0)
        {
          this -> rainbow_pwm_3 -= 1 ;
        }

        if (this->rainbow_pwm_1 == 255 && this->rainbow_pwm_2 == 0 && this->rainbow_pwm_3 == 0)
        {
          this -> rainbow_cycle_ident += 1 ;
        }

        this -> rainbow_timer = millis() ;

        if (this-> rainbow_cycle_ident == RAINBOW_CYCLES)
        {
          this -> rainbow_cycle_ident = 0 ;
          this -> lightshow_mode_ident += 1 ;
          this -> all_off() ;
        }
      }
    }


    void blink_single()
    {
      /*
       * Both LEDs blink alternately with switching colors
       */
      if (!this->blink_single_is_on && millis() >= this->blink_single_timer + BLINK_SINGLE_OFF)
      {
        if (this->blink_single_mode_ident == 0)
        {
          this -> left  -> write(255, 0, 0) ;
        }
        else if (this->blink_single_mode_ident == 1)
        {
          this -> right -> write(255, 60, 0) ;
        }
        else if (this->blink_single_mode_ident == 2)
        {
          this -> left  -> write(255, 200, 0) ;
        }
        else if (this->blink_single_mode_ident == 3)
        {
          this -> right -> write(100, 220, 0) ;
        }
        else if (this->blink_single_mode_ident == 4)
        {
          this -> left  -> write(40, 0, 255) ;
        }
        else if (this->blink_single_mode_ident == 5)
        {
          this -> right -> write(0, 0, 255) ;
        }
        else if (this->blink_single_mode_ident == 6)
        {
          this -> left  -> write(0, 255, 0) ;
        }
        else if (this->blink_single_mode_ident == 7)
        {
          this -> right -> write(0, 255, 40) ;
        }
        else if (this->blink_single_mode_ident == 8)
        {
          this -> left  -> write(0, 255, 255) ;
        }
        else if (this->blink_single_mode_ident == 9)
        {
          this -> right -> write(42, 255, 0) ;
        }
        else if (this->blink_single_mode_ident == 10)
        {
          this -> left  -> write(120, 0, 255) ;
        }
        else if (this->blink_single_mode_ident == 11)
        {
          this -> right -> write(0, 120, 160) ;
        }

        this->blink_single_is_on = true ;
        this->blink_single_timer = millis() ;
      }
      
      else if (this->blink_single_is_on && millis() >= this->blink_single_timer + BLINK_SINGLE_ON)
      {
        this -> all_off() ;
        
        this->blink_single_is_on = false ;
        
        this->blink_single_mode_ident += 1 ;

        this->blink_single_timer = millis() ;

        if (this->blink_single_mode_ident == 12)
        {
          this->blink_single_mode_ident = 0 ;
          this->blink_single_cycle_ident += 1 ;

          if (this->blink_single_cycle_ident == BLINK_SINGLE_CYCLES)
          {
            this -> blink_single_cycle_ident = 0 ;
            this -> lightshow_mode_ident += 1 ;
            this -> all_off() ;
          }
        }
      }
    }


    void rainbow_splitted()
    {   
      /*
       * Both LEDs walk through the whole Hue spectrum while blinking alternately
       */
      if (millis() >= this->rainbow_splitted_switch_timer + RAINBOW_SPLITTED_SWITCH_TIME)
      {
        this -> rainbow_splitted_is_left = !this -> rainbow_splitted_is_left ;
        this->rainbow_splitted_switch_timer = millis() ;
      }
      
      if (millis() >= this->rainbow_timer + this->rainbow_splitted_timeout)
      {
        if (this->rainbow_splitted_is_left)
        {
          this -> left  -> write(rainbow_pwm_1, rainbow_pwm_2, rainbow_pwm_3) ;
          this -> right -> write(0, 0, 0) ;
        }
        else
        {
          this -> right -> write(rainbow_pwm_1, rainbow_pwm_2, rainbow_pwm_3) ;
          this -> left  -> write(0, 0, 0) ;
        }
        
        if (this->rainbow_pwm_1 == 255 && this->rainbow_pwm_2 < 255 && this->rainbow_pwm_3 == 0)
        {
          this -> rainbow_pwm_2 += 1 ;
        }
        else if (this->rainbow_pwm_1 > 0 && this->rainbow_pwm_2 == 255 && this->rainbow_pwm_3 == 0)
        {
          this -> rainbow_pwm_1 -= 1 ;
        }
        else if (this->rainbow_pwm_1 == 0 && this->rainbow_pwm_2 == 255 && this->rainbow_pwm_3 < 255)
        {
          this -> rainbow_pwm_3 += 1 ;
        }
        else if (this->rainbow_pwm_1 == 0 && this->rainbow_pwm_2 > 0 && this->rainbow_pwm_3 == 255)
        {
          this -> rainbow_pwm_2 -= 1 ;
        }
        else if (this->rainbow_pwm_1 < 255 && this->rainbow_pwm_2 == 0 && this->rainbow_pwm_3 == 255)
        {
          this -> rainbow_pwm_1 += 1 ;
        }
        else if (this->rainbow_pwm_1 == 255 && this->rainbow_pwm_2 == 0 && this->rainbow_pwm_3 > 0)
        {
          this -> rainbow_pwm_3 -= 1 ;
        }

        if (this->rainbow_pwm_1 == 255 && this->rainbow_pwm_2 == 0 && this->rainbow_pwm_3 == 0)
        {
          this -> rainbow_splitted_cycle_ident += 1 ;
        }


        this -> rainbow_timer = millis() ;

        if (this-> rainbow_splitted_cycle_ident == RAINBOW_SPLITTED_CYCLES)
        {
          this -> rainbow_splitted_cycle_ident = 0 ;
          this -> lightshow_cycle_ident += 1 ;
          this -> all_off() ;

          if (this->lightshow_cycle_ident == LIGHTSHOW_ALL_OVER_CYCLES)
          {
            this -> lightshow_mode_ident = 42 ;
            this -> lightshow_cycle_ident = 0 ;
          }
          else
          {
            this -> lightshow_mode_ident = 0 ;
          }
        }
      }
    }


    void white_blink_cycle_end()
    {
      /*
       * Both LEDs blink with decrementing time intervals
       * Then a fast Fade-In and Fade-Out
       */
      if (this->white_blink_mode_ident == 0)
      {
        if (millis() >= this->white_blink_timer + this->white_blink_timeout)
        {
          if (!this->white_blink_is_on)
          {
            this -> left  -> write(255, 255, 255) ;
            this -> right -> write(255, 255, 255) ;

            this -> white_blink_is_on = true ;
            this -> white_blink_timer = millis() ;
          }
          else
          {
            this -> all_off() ;
            this -> white_blink_timeout -= WHITE_BLINK_TIMEOUT_DECRMNT ;
            
            this -> white_blink_timer = millis() ;

            this -> white_blink_is_on = false ;
            
            if (this->white_blink_timeout <= WHITE_BLINK_TIMEOUT_DECRMNT)
            {
              this -> white_blink_mode_ident = 1 ;
              this -> white_blink_timeout = WHITE_BLINK_TIMEOUT_FADE ;
              this -> white_blink_is_on = true ;
            }
          }
        }
      }
      
      else if (this->white_blink_mode_ident == 1)
      {
        if (millis() >= this->white_blink_timer + this->white_blink_timeout)
        {
          this -> left  -> write(this->white_blink_pwm, this->white_blink_pwm, this->white_blink_pwm) ;
          this -> right -> write(this->white_blink_pwm, this->white_blink_pwm, this->white_blink_pwm) ;

          if (this->white_blink_pwm == 255)
          {
            this -> white_blink_is_on = false ;
          }
          
          if (this->white_blink_is_on)
          {
            this -> white_blink_pwm += 1 ;
          }
          else
          {
            this -> white_blink_pwm -= 1 ;
            
            if (this->white_blink_pwm == 0)
            {
              this -> all_off() ;
              this -> white_blink_timeout = WHITE_BLINK_TIMEOUT_BEGIN ;
              this -> white_blink_mode_ident = 0 ;
              this -> lightshow_mode_ident = 0 ;
            }
          }

          this -> white_blink_timer = millis() ;
        }
      }
    }


    void all_off()
    {
      /*
       * Turns both LEDs off
       */
      this -> left  -> off() ;
      this -> right -> off() ;
    }


    void calc_parameters()
    {
      /*
       * Calculates parameters for the lightshow from global parameter definitions
       */
      this -> rainbow_timeout = RAINBOW_CYCLE_TIME * 1000 / 1530 ;
      this -> rainbow_splitted_timeout = RAINBOW_SPLITTED_CYCLE_TIME * 1000 / 1530 ;
    }


    void reset()
    {
      /*
       * Resets variables of the class to initial settings
       */
      if (!this->is_resetted)
      {
        this -> lightshow_mode_ident = 0 ;
        this -> lightshow_cycle_ident = 0 ;
  
  
        this -> blink_double_mode_ident = 0 ;
        this -> blink_double_cycle_ident = 0 ;
        
        this -> blink_double_is_on = false ;
    
        
        this -> rainbow_cycle_ident = 0 ;
        
        this -> rainbow_pwm_1 = 255 ;
        this -> rainbow_pwm_2 = 0 ;
        this -> rainbow_pwm_3 = 0 ;
        
        this -> rainbow_splitted_cycle_ident = 0 ;
    
        this -> rainbow_splitted_is_left = false ;
  
   
        this -> blink_single_mode_ident = 0 ;
        this -> blink_single_cycle_ident = 0 ;
        this -> blink_single_is_on = false ;
        this -> blink_single_is_left = true ;
     
        
        this -> white_blink_mode_ident = 0 ;
        this -> white_blink_is_on = false ;
  
        this -> white_blink_pwm = 0 ;

        this -> is_resetted = true ;
      }
    }

    RgbLed*   left ;
    RgbLed*   right ;

    uint8_t   lightshow_mode_ident = 0 ;
    uint8_t   lightshow_cycle_ident = 0 ;

    bool      is_resetted = true ;


    uint8_t   blink_double_mode_ident = 0 ;
    uint8_t   blink_double_cycle_ident = 0 ;

    bool      blink_double_is_on = false ;
    uint64_t  blink_double_timer = 0 ;


    uint8_t   rainbow_cycle_ident = 0 ;
    
    uint8_t   rainbow_pwm_1 = 255 ;
    uint8_t   rainbow_pwm_2 = 0 ;
    uint8_t   rainbow_pwm_3 = 0 ;
    
    uint64_t  rainbow_timer = 0 ;
    uint32_t  rainbow_timeout ;

    uint8_t   rainbow_splitted_cycle_ident = 0 ;
    
    uint32_t  rainbow_splitted_timeout ;
    
    bool      rainbow_splitted_is_left = false ;
    uint64_t  rainbow_splitted_switch_timer = 0 ;
    

    uint8_t   blink_single_mode_ident = 0 ;
    uint8_t   blink_single_cycle_ident = 0 ;
    bool      blink_single_is_on = false ;
    bool      blink_single_is_left = true ;

    uint64_t  blink_single_timer = 0 ;


    uint8_t   white_blink_mode_ident = 0 ;
    bool      white_blink_is_on = false ;

    uint64_t  white_blink_timer = 0 ;
    uint32_t  white_blink_timeout = WHITE_BLINK_TIMEOUT_BEGIN ;

    uint8_t   white_blink_pwm = 0 ;
    
} ;




Stepper           finger        ( STEPPER_STP , STEPPER_DIR , STEPPER_EN ) ;

RgbLed            rgb_led_l     ( RGB_L_RED , RGB_L_GRN , RGB_L_BLU ) ;
RgbLed            rgb_led_r     ( RGB_R_RED , RGB_R_GRN , RGB_R_BLU ) ;

Lightshow         lightshow     ( &rgb_led_l , &rgb_led_r ) ;


#ifdef WEBSERVER
  WebServerManager  input_control ( &server ) ;
#endif

#ifdef UDP_SERVER
  UdpServerManager  input_control ( &server ) ;
#endif


WifiManager       wifi_management           ;




void setup()
{
  wifi_management.init() ;
  wifi_management.check() ;
  
  input_control.init(_SERVER_PORT) ;
}



void loop()
{
  wifi_management.check() ;
  
  input_control.work() ;
  
  finger.work() ;
  lightshow.work() ;
}
