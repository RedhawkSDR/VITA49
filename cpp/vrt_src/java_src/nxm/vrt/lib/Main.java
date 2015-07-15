/**
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of REDHAWK.
 *
 * REDHAWK is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * REDHAWK is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

package nxm.vrt.lib;

import java.lang.reflect.Constructor;
import java.util.Arrays;
import java.util.Collections;
import java.util.Formatter;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

/** <b>Command-Line Use Only:</b> Main method for the VRT Java Library which simply
 *  prints out usage information. <br>
 *  <br>
 *  This class is also used internally as a super-class by other functions with a
 *  main method.
 *
 *  @author 
 */
public class Main {
  private static final String[] OPTIONS = { "" };
  private static final String[] VERSION = { "VRT Java Library "+VRTConfig.LIBRARY_VERSION };
  private static final String[] USAGE   = {
      "This is the core of the VRT Java Library and is intended to be linked into",
      "larger applications. This JAR file also includes the following stand-alone",
      "utility functions:",
      "",
      "PacketGen - Packet generation tool for Java and C++. For more information",
      "            please run:",
      "              java -classpath <classpath> nxm.vrt.libg.PacketGen --help",
      "",
      "NICCheck  - Tool for identifying the networks connected to by the local",
      "            system. For more information please run:",
      "              java -classpath <classpath> nxm.vrt.net.NICCheck --help",
      "",
      "UDPDump   - Tool for monitoring/checking UDP streams (including multicast)",
      "            which can extract and display received data as either raw",
      "            traffic or as VRT packets. Unlike 'tcpdump' this does not",
      "            require administrative privilages to run. For more information",
      "            please run:",
      "              java -classpath <classpath> nxm.vrt.net.UDPDump --help",
  };
  
  public  static final String   ETC          = ""; // "" and "-" can not be valid options, use as flags
  private static final String[] TRUE_VALUES  = { "", "1", "T", "TRUE",  "Y", "YES", "ON" };
  private static final String[] FALSE_VALUES = {     "0", "F", "FALSE", "N", "NO",  "OFF"};
  
  static {
    // Code elsewhere requires the TRUE_VALUES and FALSE_VALUES to be sorted
    Arrays.sort(TRUE_VALUES);
    Arrays.sort(FALSE_VALUES);
  }
  
  protected final String[]           version;     // The version information.
  protected final String[]           usage;       // The usage information.
  protected final Map<String,String> options;     // Options passed in on the command line (unmodifiable)
  protected final List<String>       parameters;  // Parameters passed in on the command line (unmodifiable)
  

  /** <b>Internal Use Only:</b> Creates a new instance.
   *  @param args The command-line arguments.
   */
  public Main (String[] args) {
    this(VERSION, USAGE, OPTIONS, 0, 0, args);
  }
  
  /** Creates a new instance.
   *  @param version          The version information.
   *  @param usage            The usage information.
   *  @param supportedOptions List of supported options. See {@link #parseCmdLine} for details.
   *  @param minArgs          Minimum number of arguments.
   *  @param maxArgs          Maximum number of arguments (-1=Unlimited).
   *  @param args             The user's arguments passed in on the command line.
   *  @throws InvalidCommandLineException If there is a syntax error on the command line.
   */
  protected Main (String[] version, String[] usage, String[] supportedOptions, int minArgs, int maxArgs, String[] args) {
    Map<String,String> opts   = new LinkedHashMap<String,String>();
    List<String>       params = new LinkedList<String>();

    parseCmdLine(opts, params, supportedOptions, false, true, minArgs, maxArgs, args);

    this.version    = version;
    this.usage      = usage;
    this.options    = Collections.unmodifiableMap(opts);
    this.parameters = Collections.unmodifiableList(params);
  }

  /** Is the help/version flag set? */
  protected boolean isHelpVersion () {
    return isHelpVersion(options);
  }

  /** Is the help/version flag set? */
  private static boolean isHelpVersion (Map<String,String> opts) {
    return opts.containsKey("-H") || opts.containsKey("--help")
        || opts.containsKey("-V") || opts.containsKey("--version")
        || opts.containsKey("--dumpcshscript");
  }
  
  /** Parses the command line.
   *  @param opts             The map to insert the options into.
   *  @param params           The list to add the parameters to.
   *  @param supportedOptions List of supported options. Arguments starting with a single dash
   *                          (e.g. "-n") are single-letter options and can be combined (e.g.
   *                          "-np" is the same as "-n -p"). Arguments starting with a double dash
   *                          (e.g. "--version") are whole words and can not be combined like the
   *                          other ones. Options that end in a equals (e.g. "-n=") expect an
   *                          accompanying parameter (e.g. "-n 20" or "-n=20" for "-n="). An
   *                          unaccompanied double dash (i.e. "--") indicated the end of the options
   *                          list and all following arguments are treated as parameters. If the
   *                          list of supported options ends with {@link #ETC}, additional options
   *                          will be accepted without error, however only minimal error checking
   *                          will be done and users are strongly encouraged to use "state value"
   *                          with an equal sign (e.g. "--verbose=1" not "--verbose") or end the
   *                          list of options with "--" as failure to do so could result in the
   *                          first parameter being misidentified as the value of an option (e.g.
   *                          "-l xyz" will be treated as "-l=xyz" not "-l" followed by parameter
   *                          "xyz"). Also note that additional state value options are not
   *                          pre-checked for true/false values like normal options are.
   *  @param joinOptions      Should this allow joining of single character options (e.g. handle
   *                          "-ab" as if the user entered "-a -b")?
   *  @param helpVer          Print out help ("-H" or "--help") and version ("-V" or "--version")
   *                          information and sets initStatus=Finish.
   *  @param minArgs          Minimum number of arguments.
   *  @param maxArgs          Maximum number of arguments (-1=Unlimited).
   *  @param args             The user's arguments passed in on the command line.
   *  @throws InvalidCommandLineException If there is a syntax error on the command line.
   */
  private static void parseCmdLine (Map<String,String> opts, List<String> params,
                                    String[] supportedOptions, boolean joinOptions, boolean helpVer,
                                    int minArgs, int maxArgs, String ... args) {
    if (helpVer) {
      int len = supportedOptions.length;
      supportedOptions = Arrays.copyOf(supportedOptions, len+5);
      supportedOptions[len++] = "-V";
      supportedOptions[len++] = "--version";
      supportedOptions[len++] = "-H";
      supportedOptions[len++] = "--help";
      supportedOptions[len++] = "--dumpcshscript";
    }
    else {
      int len = supportedOptions.length;
      supportedOptions = Arrays.copyOf(supportedOptions, len+5);
      supportedOptions[len++] = "--dumpcshscript";
    }

    int index = 0;
    for (; index < args.length; index++) {
      String arg = args[index];
      if (arg.equals("--")) {
        break;
      }
      else if (arg.startsWith("--")) {
        index = addOption(opts, arg, args, index, supportedOptions);
      }
      else if (arg.startsWith("-")) {
        if ((!joinOptions) || (arg.length() <= 2)) {
          index = addOption(opts, arg, args, index, supportedOptions);
        }
        else {
          for (int i = 1; i < arg.length(); i++) {
            addOption(opts, "-"+arg.charAt(i), null, index, supportedOptions);
          }
        }
      }
      else {
        break;
      }
    }
    for (; index < args.length; index++) {
      params.add(args[index]);
    }

    // CHECK REQUIRED OPTIONS
    if (!helpVer || !isHelpVersion(opts)) {
      for (String opt : supportedOptions) {
        if (!opt.endsWith("*")) continue; //not a required option
        String key = (opt.endsWith("=*"))? opt.substring(0, opt.length()-2)
                                         : opt.substring(0, opt.length()-1);
        if (!opts.containsKey(key)) {
          throw new InvalidCommandLineException("Missing required option "+key);
        }
      }
    }

    // CHECK REQUIRED NUMBER OF PARAMETERS
    if (!helpVer || !isHelpVersion(opts)) {
      if (params.size() < minArgs) {
        throw new InvalidCommandLineException("Incorrect number of parameters");
      }
      if ((maxArgs >= 0) && (params.size() > maxArgs)) {
        throw new InvalidCommandLineException("Incorrect number of parameters");
      }
    }
  }

  /** Only used by {@link #parseCmdLine}. */
  private static int addOption (Map<String,String> opts, String arg, String[] args, int index,
                                String[] supportedOptions) {
    String  key = arg;
    String  val = null;
    boolean etc = false;
    int equals = arg.indexOf('=');
    if (equals > 0) {
      key = arg.substring(0,equals);
      val = arg.substring(equals+1);
    }
    
    for (String opt : supportedOptions) {
      if (opt.equals(ETC))   { etc = true; continue; }
      if (opt.endsWith("*")) { opt = opt.substring(0,opt.length()-1); } // marks a required option

      boolean hasValue = false;
      if (opt.endsWith("=")) {
        hasValue = true;
        opt = opt.substring(0,opt.length()-1);
      }
      
      if (opt.equals(key)) {
        if (!hasValue) { // state value
          if (val == null)                                               { opts.put(key, ""); return index; } // present
          if (Arrays.binarySearch(TRUE_VALUES,  val.toUpperCase()) >= 0) { opts.put(key, ""); return index; } // true
          if (Arrays.binarySearch(FALSE_VALUES, val.toUpperCase()) >= 0) {                    return index; } // false
          throw new InvalidCommandLineException("Invalid syntax for option "+arg);
        }
        if (val != null) { // --key=val
          opts.put(key, val);
          return index;
        }
        if ((args != null) && (args.length > index+1)) { // --key val
          opts.put(key, args[index+1]);
          return index+1;
        }
        throw new InvalidCommandLineException("Invalid syntax for option "+arg);
      }
    }

    // Handle case of supportedOptions with ETC
    if (etc) {
      if (val != null) { // explicit --key=val
        opts.put(key, val);
        return index;
      }
      if ((args != null) && (args.length > index+1) && !args[index+1].startsWith("-")) { // assume --key val
        opts.put(key, args[index+1]);
        return index+1;
      }

      // assume state value
      opts.put(key, "");
      return index;
    }
    
    throw new InvalidCommandLineException("Invalid option "+arg);
  }

  /** Gets a user-entered option, returns <tt>false</tt> if not entered. */
  protected boolean isOptionSet (String key) { return options.containsKey(key); }
  /** Gets a user-entered option, returns <tt>null</tt> if not entered. */
  protected String getOption (String key) { return getOption(key, null); }
  /** Gets a user-entered option as a string, returns <tt>null</tt> if not entered or "". */
  protected String getStringOption (String key) { return getStringOption(key, null); }
  /** Gets a user-entered option as a number, returns <tt>null</tt> if not entered. */
  protected Number getNumericOption (String key) { return getNumericOption(key, (Double)null); }

  /** Gets a user-entered option, returns <tt>def</tt> if not entered. */
  protected String getOption (String key, String def) {
    String val = options.get(key);
    return (val != null)? val : def;
  }

  /** Gets a user-entered option as a string, returns <tt>def</tt> if not entered or "". */
  protected String getStringOption (String key, String def) {
    String val = options.get(key);
    return ((val == null) || (val.length() == 0))? def : val;
  }

  /** Gets a user-entered option as a number, returns <tt>def</tt> if not entered or "". */
  @SuppressWarnings(value={"unchecked"})
  protected <T extends Enum<T>> T getEnumOption (String key, T def) {
    return getEnumOption(key, def, (Class<T>)def.getClass());
  }

  /** Gets a user-entered option as a number, returns <tt>null</tt> if not entered or "". */
  protected <T extends Enum<T>> T getEnumOption (String key, Class<T> clazz) {
    return getEnumOption(key, null, clazz);
  }

  /** Gets a user-entered option as a number, returns <tt>null</tt> if not entered or "". */
  @SuppressWarnings(value={"unchecked"})
  private <T extends Enum<T>> T getEnumOption (String key, T def, Class<T> clazz) {
    String val = options.get(key);
    if ((val == null) || (val.length() == 0)) return def;
    try {
      return Enum.valueOf(clazz, val);
    }
    catch (IllegalArgumentException e) {
      Object[] values;
      try {
        values = (Object[])clazz.getMethod("values").invoke(null);
      }
      catch (Exception ex) {
        throw new AssertionError("Could not access values() in "+clazz+": "+ex);
      }
      throw new InvalidCommandLineException("Value of "+key+"= must be one of "+Arrays.toString(values)+", but given "+val);
    }
  }

  /** Gets a user-entered option as a number, returns <tt>def</tt> if not entered. */
  protected Integer getNumericOption (String key, Integer def) {
    String val = options.get(key);
    if (val == null) return def;
    try {
      return Integer.parseInt(val);
    }
    catch (NumberFormatException e) {
      throw new InvalidCommandLineException("Value of "+key+"= must be numeric (integer), but given "+val);
    }
  }

  /** Gets a user-entered option as a number, returns <tt>def</tt> if not entered. */
  protected Long getNumericOption (String key, Long def) {
    String val = options.get(key);
    if (val == null) return def;
    try {
      return Long.parseLong(val);
    }
    catch (NumberFormatException e) {
      throw new InvalidCommandLineException("Value of "+key+"= must be numeric (integer), but given "+val);
    }
  }

  /** Gets a user-entered option as a number, returns <tt>def</tt> if not entered. */
  protected Float getNumericOption (String key, Float def) {
    String val = options.get(key);
    if (val == null) return def;
    try {
      return Float.parseFloat(val);
    }
    catch (NumberFormatException e) {
      throw new InvalidCommandLineException("Value of "+key+"= must be numeric (floating-point), but given "+val);
    }
  }

  /** Gets a user-entered option as a number, returns <tt>def</tt> if not entered. */
  protected Double getNumericOption (String key, Double def) {
    String val = options.get(key);
    if (val == null) return def;
    try {
      return Double.parseDouble(val);
    }
    catch (NumberFormatException e) {
      throw new InvalidCommandLineException("Value of "+key+"= must be numeric (floating-point), but given "+val);
    }
  }
  
  /** Creates a csh script that will run this command. */
  protected CharSequence getScriptCSH () {
    Formatter str = new Formatter();
    
    String vrthome = System.getProperty("VRTHOME",   "${VRTHOME}");
    String flags   = System.getProperty("FLAGS",     "");
    String jarDir  = System.getProperty("JARDIR",    vrthome+"/java_lib");
    String javaCmd = flags + " -classpath ${classpath} "
                           + getClass().getCanonicalName();

    str.format("#!/bin/csh%n");
    str.format("# This file is auto-generated, do not edit%n");
    str.format("#%n");
    str.format("if ($?VRTHOME) then%n");
    str.format("  setenv classpath ${VRTHOME}/java_lib/vrt.jar%n");
    str.format("else%n");
    str.format("  setenv classpath %s/vrt.jar%n", jarDir);
    str.format("endif%n");
    str.format("%n");
    str.format("if ($?JAVAHOME) then%n");
    str.format("  ${JAVAHOME}/bin/java  %s $*%n", javaCmd);
    str.format("else if ($?JAVA_HOME) then%n");
    str.format("  ${JAVA_HOME}/bin/java %s $*%n", javaCmd);
    str.format("else%n");
    str.format("  java                  %s $*%n", javaCmd);
    str.format("endif%n");
    
    return str.toString();
  }
  
  /** Does the body of the work for this "main" method. This should be accompanied
   *  by a main method similar to the following (MyClass is the name of the class):
   *  <pre>
   *    public static void main (String ... args) {
   *      runMain(MyClass.class, args);
   *    }
   *  </pre>
   *  @throws Exception If an exception occurs.
   */
  @SuppressWarnings("UseOfSystemOutOrSystemErr")
  protected void main () throws Exception {
    // Default implementation simply prints usage information
    for (String s : USAGE) {
      System.out.println(s);
    }
  }
  
  /** Default implementation. */
  public static void main (String ... args) {
    runMain(Main.class, args);
  }
  
  /** Runs a new instance.
   *  @param clazz            The type to run. The class must have a 1-argument
   *                          constructor that takes in an <tt>String[]</tt> 
   *                          which is equal to <tt>args</tt>.
   *  @param args             The user's arguments passed in on the command line.
   *  @throws InvalidCommandLineException If there is a syntax error on the command line.
   */
  @SuppressWarnings({"UseOfSystemOutOrSystemErr","CallToPrintStackTrace"})
  protected static <T extends Main> void runMain (Class<T> clazz, String ... args) {
    try {
      Constructor<T> c = clazz.getConstructor(String[].class);
      T              m = c.newInstance((Object)args);
      
      
      if (m.options.containsKey("-H") || m.options.containsKey("--help")) {
        for (String s : m.usage) {
          System.out.println(s);
        }
      }
      else if (m.options.containsKey("-V") || m.options.containsKey("--version")) {
        for (String s : m.version) {
          System.out.println(s);
        }
      }
      else if (m.options.containsKey("--dumpcshscript")) {
        System.out.println(m.getScriptCSH());
      }
      else {
        m.main();
      }
    }
    catch (InvalidCommandLineException e) {
      System.err.println(e.getMessage());
      for (String s : USAGE) {
        System.err.println(s);
      }
      System.exit(-1);
    }
    catch (Throwable t) {
      System.err.println("An unexpected error occurred: "+t.getMessage());
      t.printStackTrace();
      System.exit(-1);
    }
  }
  
  /** A command-line exception occurred. */
  public static class InvalidCommandLineException extends IllegalArgumentException {
    private static final long serialVersionUID = 0x2010020400000001L; // 2010-02-04 rev 00000001

    public InvalidCommandLineException (String msg) {
      super(msg);
    }
  }
}
