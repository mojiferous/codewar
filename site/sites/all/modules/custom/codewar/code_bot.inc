<?php
/**
 * code_bot.php
 * defines the codebot class
 * 12/20/12 Mojiferous
 */

require_once('evalmath/evalmath.class.php');

class code_bot {

  //many of these variables are public so that they store correctly in a $_SESSION variable
  private $body_parts = array(); //< the raw code array
  public $code_array = array(); //< the compiled code array
  public $line_num; //< integer value of current line number
  public $call_stack = array(); //< array of line numbers that the code calls back to, used for gosub commands
  private $command_array = array(
    'goto',
    'gosub',
    'if',
    'endsub',
  ); //< array of language commands
  private $sys_vars = array(
    '_x',
    '_y',
    '_hp',
  );

  /**
   * object constructor
   * @param string $raw_code
   */
  public function __construct($raw_code) {
    $this->body_parts = $raw_code;
    $this->line_num = 0;
  }

  /**
   * rebuild a code bot from cached data
   * @param $cached_data
   */
  public function rebuild_from_cache($cached_data) {
    if(isset($cached_data->code_array) && isset($cached_data->line_num) && isset($cached_data->call_stack)) {
      $this->code_array = $cached_data->code_array;
      $this->line_num = $cached_data->line_num;
      $this->call_stack = $cached_data->call_stack;
    }
  }

  /**
   * run the bots code
   */
  public function run_code() {
    $this->check_code_array();

    if(isset($this->code_array) && count($this->code_array['code']) > 0) {
      //we actually have something here

      $ret_val = array();

      $zero_counter = 0;

      do {
        //skip over empty lines, which the compiler spits out to allow for proper error checking

        //reset the counter back to 0
        if($this->line_num >= count($this->code_array['code'])) {
          $this->line_num = 0;
          $zero_counter++;

          if($zero_counter > 1) {
            //we've gone through this code more than once, set and error and break the while loop
            _set_array_error($ret_val, 'Your code is nothing but a bunch of empty lines -- existentialism doesn\'t really work here.');
            break;
          }
        }

        $line_val = $this->code_array['code'][$this->line_num];
        $this->line_num++;

      } while (empty($line_val));

      $ret_val['line_num'] = $this->line_num-1;

      dpm(json_encode($line_val), 'this line'); dpm($this->code_array, 'full array');

      if(isset($line_val[0])) {
        $this->run_line($line_val, $ret_val);
      } elseif ($zero_counter == 0) {
        //for some reason the code here was not parsed correctly, should not happen, but if it does, break here
        _set_array_error($ret_val, 'Unable to parse code at line #'.$this->line_num);
      }

      return $ret_val;
    }

    return array('ERROR' => 'No code!');
  }

  /**
   * actual processing function for a line of code
   * @param string $line_val
   * @param string|int $ret_val
   */
  private function run_line($line_val, &$ret_val) {
    if(isset($line_val[0])) {

      switch($line_val[0]) {
        case 'assign':
          //variable assignment
          if(isset($line_val[1]) && isset($line_val[2])) {
            //make sure we have values for the assignment

            //trim blank space
            $var_name = trim($line_val[1]);
            $var_val = trim($line_val[2]);

            if(isset($this->code_array['vars']->$var_name)) {

              if(!is_numeric($var_val)) {
                //if the value being assigned is not numeric, figure out what is going on here
                if(!$this->eval_code($var_val)) {
                  //there has been an error!
                  _set_array_error($ret_val, 'Assignment error on line #'.$ret_val['line_num'].'. '.$var_val);

                  $var_val = false;
                }
              }

              if($var_val !== false) {
                //there was a value assigned, otherwise, something went wrong and we set and error

                if(in_array($var_name, $this->sys_vars)) {
                  //this is a system value, we can not assign a value to this within the program
                  _set_array_error($ret_val, 'Attempt to assign value to system variable '.$var_name.' on line #'.$ret_val['line_num']);

                } else {
                  //this is a user var, assign it
                  $this->code_array['vars']->$var_name = $var_val;

                  $ret_val['assigned '.$var_name] = $var_val;
                }


              }


            } else {
              //the variable you are assigning does not exist
              _set_array_error($ret_val, 'Invalid variable assignment on line #'.$ret_val['line_num'].'. Variable '.$var_name.' does not exist');
            }

          } else {
            //something isn't right here
            _set_array_error($ret_val, 'Invalid assignment or code on line #'.$ret_val['line_num']);
          }

          break;
        case 'endsub':
          //end a sub function and jump back to where you were
          if(isset($this->call_stack) && count($this->call_stack) > 0) {
            //reverse the call stack, since we want the last position
            array_reverse($this->call_stack);

            $this->line_num = array_shift($this->call_stack);

            //re-reverse the array to put it back into the correct order
            array_reverse($this->call_stack);
          } else {
            //there is nothing in the call stack
            _set_array_error($ret_val, 'Stack underflow error');
          }

          break;
        case 'goto':
          //jump to new function without return ability

          $func_name = trim($line_val[1]);

          if(isset($line_val[1])) {
            if(isset($this->code_array['funcs']->$func_name)) {
              //the next line will be the head of the function

              $this->line_num = $this->code_array['funcs']->$func_name;

            } else {
              //the function this goto was pointing to does not exist
              _set_array_error($ret_val, 'The function '.$func_name.' on line #'.$ret_val['line_num'].' does not exist');
            }

          } else {
            //there is no goto value, break here
            _set_array_error($ret_val, 'Attempt to goto nothing on line #'.$ret_val['line_num'].'. Very nihilistic.');
          }

          break;
        case 'gosub':
          //jump to sub function

          $func_name = $line_val[1];

          if(isset($line_val[1])) {
            if(isset($this->code_array['funcs']->$func_name)) {
              //the next line will be the head of the function

              //add the next line as the "return"
              $this->call_stack[] = $this->line_num++;

              $this->line_num = $this->code_array['funcs']->$func_name;

            } else {
              //the function this goto was pointing to does not exist
              _set_array_error($ret_val, 'The function '.$func_name.' on line #'.$ret_val['line_num'].' does not exist');
            }

          } else {
            //there is no goto value, break here
            _set_array_error($ret_val, 'Attempt to gosub nothing on line #'.$ret_val['line_num'].'. Very nihilistic.');
          }

          break;
        case 'if':
          //if statement
          if(isset($line_val[1])) {
            //make sure we have a test value, which will be in $line_val[1]

            //split the test value along the standard test vals
            preg_match_all("/(.*?)([<>!=]+)(.*)/is", $line_val[1], $matches);
            if(isset($matches[1][0]) && isset($matches[3][0]) && !empty($matches[1][0]) && !empty($matches[3][0])) {
              //make sure we have matches for these arguments
              $first_test = $matches[1][0];
              $second_test = $matches[3][0];

              if(!is_numeric($first_test) && !$this->eval_code($first_test)) {
                //our first logical test failed
                _set_array_error($ret_val, 'Error testing if logic on line #'.$ret_val['line_num'].'. '.$first_test);
              } else {
                //our first logical test passed
                if(!is_numeric($second_test) && !$this->eval_code($second_test)) {
                  //our second logical test failed
                  _set_array_error($ret_val, 'Error testing if logic on line #'.$ret_val['line_num'].'. '.$second_test);
                } else {
                  //both of our logical tests passed

                  if(isset($matches[2][0]) && $this->eval_logic($first_test, $second_test, $matches[2][0])) {
                    //our test is set and has passed as true, evaluate the rest of the line

                    //slice off the first two commands on this line
                    $line_val = array_slice($line_val, 2);

                    $this->run_line($line_val, $ret_val);
                  }

                }
              }

            } else {
              //this is a different logical test

              if(empty($matches[1][0]) && !empty($matches[2][0]) && !empty($matches[3][0])) {
                //this should be a ! logical test
                if($matches[2][0] == '!') {
                  if(!$this->eval_code($matches[3][0])) {
                    //the test returned false, meaning our test is actually true

                    //slice off the first two commands on this line
                    $line_val = array_slice($line_val, 2);

                    $this->run_line($line_val, $ret_val);
                  }
                }
              } else {
                //all our matches are empty -- meaning this should just be a straight logical test

                if($this->eval_code($line_val[1])) {
                  //this test was true

                  //slice off the first two commands on this line
                  $line_val = array_slice($line_val, 2);

                  $this->run_line($line_val, $ret_val);
                }
              }

            }


          }

          break;

        case 'sysfunc':
          //sets system functions
          if(isset($line_val[1]) && isset($line_val[2])) {
            $ret_val[$line_val[1].' result: '] = $this->call_sys_func($line_val[1], $line_val[2]);

            if(!$ret_val && isset($this->code_array['errors'])) {
              _set_array_error($ret_val, 'Call to system function '.$line_val[1].' on line #'.$ret_val['line_num'].' threw error '.$ret_val[$line_val[1].' result: ']);
            }
          } else {
            _set_array_error($ret_val, 'Call to system function on line #'.$ret_val['line_num'].' failed');
          }

          break;
        default:
          break;
      }

    } else {
      //for some reason the code here was not parsed correctly, should not happen, but if it does, break here
      _set_array_error($ret_val, 'Unable to parse code at line #'.$this->line_num);
    }
  }

  /**
   * evaluates logical comparisons between val1 and val2, avoiding eval() for security reasons
   * @param int $val1
   * @param int $val2
   * @param string $logic
   * @return bool
   */
  private function eval_logic($val1, $val2, $logic) {
    $ret_val = false;

    switch($logic) {
      case '==':
        if($val1 == $val2) {
          $ret_val = true;
        }
        break;
      case '>':
        if($val1 > $val2) {
          $ret_val = true;
        }
        break;
      case '>=':
        if($val1 >= $val2) {
          $ret_val = true;
        }
        break;
      case '<':
        if($val1 < $val2) {
          $ret_val = true;
        }
        break;
      case '<=':
        if($val1 <= $val2) {
          $ret_val = true;
        }
        break;
      case '!=':
        if($val1 != $val2) {
          $ret_val = true;
        }
    }

    return $ret_val;
  }

  /**
   * evaluates $result_var as a mathematical function, replacing system variables
   * @param string|int $result_var
   * @return bool
   */
  private function eval_code(&$result_var) {
    //put all our vars into an array
    $vars = (array)$this->code_array['vars'];
    //and the functions
    $funcs = (array)$this->code_array['system_funcs'];

    //split this string by mathematical symbols
    $split_arg = preg_split('/[\s\(\),+*\/-]+|[\.]\s+/', $result_var, -1, PREG_SPLIT_NO_EMPTY);

    $had_change = false;
    //before the function can be split up, we have to look for system functions to replace
    foreach ($split_arg as $argument) {
      //this should be a system function, like shoot or aim
      if (preg_match_all("/(.*?)(\\[.*?\\])/is", $argument, $matches) ) {
        if(count($matches) > 1 && isset($matches[1][0]) && isset($matches[2][0])) {
          //set values for the function call from the regex result
          $repl_value = $this->call_sys_func($matches[1][0], $matches[2][0]);

          if($repl_value === false) {
            //there was an error during the sys_func call
            if(isset($this->code_array['errors'])) {
              $result_var = $this->code_array['errors'][0];
              unset($this->code_array['errors']);
            }
            return false;
          } else {
            $result_var = str_replace($argument, $repl_value, $result_var);
            $had_change = true;
          }


        }

      }
    }

    if($had_change) {
      //there were system functions replaced here, re-split the result var by math symbols
      $split_arg = preg_split('/[\s\(\),+*\/-]+|[\.]\s+/', $result_var, -1, PREG_SPLIT_NO_EMPTY);
    }


    //split the string into mathematical operators
    $split_op = preg_split('/[a-z\d_]/', $result_var, -1, PREG_SPLIT_NO_EMPTY);

    //loop through the arguments
    foreach ($split_arg as $key => $argument) {
      if(!is_numeric($argument)) {
        if(isset($vars[$argument])) {
          //there is a variable by the name of "argument" here
          $split_arg[$key] = $vars[$argument];

        } else {
          //we don't have a value for this, function should error out
          $result_var = 'Variable not found';
          return false;
        }
      }
    }

    $final_formula = '';

    //loop through each part and build a final formula
    $arg_count = count($split_arg);
    $op_count = count($split_op);

    $max_count = $arg_count;
    if($op_count > $arg_count) $max_count = $op_count;

    for($n=0; $n<$max_count; $n++) {
      if(!isset($split_op[$n])) $split_op[$n] = '';
      if(!isset($split_arg[$n])) $split_arg[$n] = '';

      if($op_count > $arg_count) {
        //there are more operators than symbols, put them first
        $final_formula .= $split_op[$n].$split_arg[$n];
      } else {
        //there are more symbols than operators
        $final_formula .= $split_arg[$n].$split_op[$n];
      }
    }

    //rpn math evaluator, lets us avoid using eval()
    $m = new EvalMath();
    $m->suppress_errors = true;
    $result_var = $m->evaluate($final_formula);
    if($result_var === false) {
      //an error has occured
      $result_var = $m->last_error;
      return false;
    }

    return true;
  }

  /**
   * calls a system function with a value
   * @param string $func_name
   * @param string $passed_val
   * @return bool|int
   */
  private function call_sys_func($func_name, $passed_val) {
    $ret_val = false;

    preg_match_all("/\\[(.*?)\\]/is", $passed_val, $matches);
    if(isset($matches[1][0])) {
      //this value has been passed through as [val] just get what is between the braces
      $passed_val = $matches[1][0];
    }

    if(!$this->eval_code($passed_val)) {
      $this->add_error($passed_val);
      return false;
    }

    switch($func_name) {
      case '_move':
        //move the bot along cardinal direction
        break;
      case '_look':
        //look direction, returns distance to nearest target, or 0 if no target found
        $ret_val = 10;
        break;
      case '_feel':
        //"feel" direction, returns the terrain value of the direction
        break;
      case '_sword':
        //attack with the sword in direction
        break;
      case '_arrow':
        //attack with the bow and arrow in direction
        break;
      default:
        break;
    }

    return $ret_val;
  }

  /**
   * parses the body code into an array, which is then json encoded by the build function
   */
  public function parse_code() {
    $line_num = 0;
    $this->set_system_vars();
    $this->code_array['funcs'] = array();

    do {
      $this_command = $this->return_next_value();

      //every line should have a value, even if empty -- the run should just ignore count() == 0 elements
      //this allows our errors to correspond to an actual line in the code and not a compiled line
      $this->code_array['code'][$line_num] = array();

      if(!empty($this_command)) {

        $first_char = substr($this_command,0,1);
        $double_first = substr($this_command,0,2);
        $last_char = substr($this_command, -1, 1);

        if(($first_char != ';' && $double_first != '//')) {
          //this is not a comment
          if($last_char == ':') {
            //this is a function

            $func_name = substr($this_command, 0, strlen($this_command)-1);

            $this->code_array['funcs'][$func_name] = $line_num;

          } else {

            if(substr($this_command,0,3) == 'dim') {
              //this is a variable init
              $var_name = trim(str_ireplace('dim', '', $this_command));

              $this->code_array['vars'][$var_name] = 0;


            } else {
              //explode the line by spaces and switch based on
              $words = explode(' ', $this_command);

              if(in_array($words[0], $this->command_array)) {
                //these are commands, make the line an array

                //search for commands within parenthesis, which should only occur in IF statements
                if($words[0] == 'if') {
                  preg_match_all("/(.*)\\((.*)\\)(.*)/is", $this_command, $matches);
                  if(isset($matches[2][0]) && isset($matches[3][0])) {
                    //make sure we have a test and a result for the if
                    $words = array(0 =>'if', 1 => $matches[2][0]);

                    //trim the blank space before and after the result commands
                    $matches[3][0] = trim($matches[3][0]);
                    $results = explode(' ', $matches[3][0]);
                    foreach ($results as $comms) {
                      $words[] = $comms;
                    }

                  }
                }

                $this->code_array['code'][$line_num] = $words;
              } else {

                if(stripos($this_command, '=') !== false) {
                  //this is an assignment, or at least should be
                  $words = explode('=', $this_command);

                  foreach ($words as $key => $word) {
                    $words[$key] = trim($word);
                  }


                  array_unshift($words, 'assign');

                  $this->code_array['code'][$line_num] = $words;
                } else {

                  //this should be a system function, like shoot or aim
                  if (preg_match_all("/(.*?)(\\[.*?\\])/is", $this_command, $matches) ) {
                    if(count($matches) > 1 && isset($matches[1][0]) && isset($matches[2][0])) {
                      //set values for the function call from the regex result
                      $this->code_array['code'][$line_num] = array('sysfunc', $matches[1][0], str_ireplace('[', '', str_ireplace(']', '', $matches[2][0])));
                    }

                  }

                }

              }


            }


          }

        }

      }

      $line_num++;

    } while (count($this->body_parts) > 0);
  }

  /**
   * set system variables and functions
   */
  private function set_system_vars() {
    //system variables
    foreach ($this->sys_vars as $var_name) {
      $this->code_array['vars'][$var_name] = 0;
    }

    //system functions, also set with assignment -- these are similar to above, but either have a result or return a value,
    $this->code_array['system_funcs']['_move'] = 0;
    $this->code_array['system_funcs']['_look'] = 0;
    $this->code_array['system_funcs']['_feel'] = 0;
    $this->code_array['system_funcs']['_sword'] = 0;
    $this->code_array['system_funcs']['_arrow'] = 0;
  }

  /**
   * adds an error to the code array
   * @param string $error_string
   */
  private function add_error($error_string) {
    $this->code_array['errors'][] = $error_string.' on line #'.($this->line_num-1);
  }

  /**
   * checks for stack underflow errors
   * @return bool
   */
  private function check_array_length() {
    if(count($this->body_parts) > 0) {
      return true;
    }

    return false;
  }

  /**
   * return the first (next) value of the code array
   * @return string
   */
  private function return_next_value() {
    if($this->check_array_length()) {
      return strtolower(trim(array_shift($this->body_parts)));
    }

    return '';
  }

  /**
   * decode the code array if it is still encoded as json
   */
  private function check_code_array() {
    if(is_array($this->code_array) && count($this->code_array) == 1 && is_string($this->code_array[0])) {
      //the code array is still a string
      if(is_string($this->code_array[0])) {
        $this->code_array = json_decode($this->code_array[0]);
      }
    }

    if(!is_array($this->code_array)) {
      $this->code_array = (array)$this->code_array;
    }

  }

  /**
   * set the bots location
   * @param int $x
   * @param int $y
   */
  public function set_location($x, $y) {
    $this->check_code_array();

    $this->code_array['vars']->_x = $x;
    $this->code_array['vars']->_y = $y;
  }

  /**
   * sets the bot hp
   * @param int $value
   */
  public function set_hp($value) {
    $this->check_code_array();

    $this->code_array['vars']->_hp = $value;
  }

  /**
   * returns array of current location
   * @return array
   */
  public function return_location() {
    $this->check_code_array();
    $ret_val = array();

    $ret_val['x'] = $this->code_array['vars']->_x;
    $ret_val['y'] = $this->code_array['vars']->_y;

    return $ret_val;
  }

  /**
   * returns a bots current hp
   * @return mixed
   */
  public function return_hp() {
    $this->check_code_array();
    return $this->code_array['vars']->_hp;
  }

}

/**
 * simple helper function to set an error value in the return array when running code
 * @param array $val_array
 * @param string $error_code
 */
function _set_array_error(&$val_array, $error_code) {
  $val_array['ERROR'] = $error_code;
}
