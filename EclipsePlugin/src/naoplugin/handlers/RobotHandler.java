package naoplugin.handlers;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.URISyntaxException;
import java.util.Date;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.Platform;
import org.eclipse.ui.console.ConsolePlugin;
import org.eclipse.ui.console.IConsole;
import org.eclipse.ui.console.IConsoleManager;
import org.eclipse.ui.console.MessageConsole;
import org.eclipse.ui.console.MessageConsoleStream;
import org.melanee.core.models.plm.PLM.PLMFactory;
import org.melanee.textdsl.modeleditor.TextualVisualizationService;
import org.melanee.textdsl.modeleditor.editor.sourceviewerconfiguration.MultiLevelModelColorConstants;
import org.melanee.textdsl.modeleditor.editor.sourceviewerconfiguration.MultiLevelModelPartitionScanner;
import org.melanee.textdsl.modeleditor.editor.sourceviewerconfiguration.MultilevelLiteralScanner;
import org.melanee.textdsl.modeleditor.textualdslmodelinterpreter.TextualDSLModelInterpreter;
import org.melanee.textdsl.models.weaving.M2TWeaving.M2TWeavingFactory;
import org.osgi.framework.Bundle;

/**
 * Robot Handler extends AbstractHandler, an IHandler base class.
 * 
 * @see org.eclipse.core.commands.IHandler
 * @see org.eclipse.core.commands.AbstractHandler
 */
public class RobotHandler extends AbstractHandler {

	public static final String MELANEE_OUTPUT = "D:\\Mannheim\\TeamProject\\Team-Project\\NAO_API\\VS_project\\script.cpp";
	public static final String BUILD_TOOLS = "D:\\Mannheim\\TeamProject\\Team-Project\\NAO_API\\VS_project";
	public static final String BUILD_COMMAND = "qibuild make --release";
	public static final String EXECUTION_FOLDER = "D:\\Mannheim\\TeamProject\\Team-Project\\NAO_API\\VS_project\\build-naoqi-sdk\\sdk\\bin";
	public static final String EXECUTION_COMMAND = "apidemonstration.exe --pip 127.0.0.1 --pport 9559";

	/**
	 * The constructor.
	 */
	public RobotHandler() {
	}

	/**
	 * the command has been executed, so extract extract the needed information
	 * from the application context.
	 */
	public Object execute(ExecutionEvent event) throws ExecutionException {

		MessageConsole myConsole = findConsole("Nao SDK");
		myConsole.activate();
		MessageConsoleStream out = myConsole.newMessageStream();

		try {
			// setup console
			out.println("Compiling for Nao ...");

			// write file
			PrintWriter writer = writeFile(MELANEE_OUTPUT);

			// template pre
			writer.println("#include \"apidemonstration.h\"");
			writer.println("// Melanee triggered build");
			writer.println("// ------------------------");
			writer.println("// " + new Date().toGMTString());
			writer.println("void APIDemonstration::script()");
			writer.println("{");

			// get melanee output
			TextualDSLModelInterpreter interpreter = new TextualDSLModelInterpreter(
					new MultiLevelModelPartitionScanner(),
					new MultilevelLiteralScanner(),
					new MultiLevelModelColorConstants(),
					M2TWeavingFactory.eINSTANCE.createWeavingModel());
			String textFromModel = interpreter
					.getTextFromModel(PLMFactory.eINSTANCE.createEntity());
			writer.println(textFromModel);

			// writer.println("agree();");

			// template post
			writer.println("}");

			writer.close();

			// invoke build
			executeCommand(BUILD_COMMAND, new File(BUILD_TOOLS));

			// execute run on NAO
			executeCommand(EXECUTION_COMMAND, new File(EXECUTION_FOLDER));
		} catch (Exception e) {
			e.printStackTrace();
			out.println(e.toString());
		}
		return null;
	}

	private PrintWriter writeFile(String path) throws Exception {
		// write file
		File target = new File(path);
		PrintWriter writer = new PrintWriter(target.toString(), "UTF-8");
		return writer;
	}

	private File resolveBundleFile(String path) throws URISyntaxException,
			IOException {
		Bundle bundle = Platform.getBundle("NaoPlugin");
		return new File(FileLocator.resolve(bundle.getEntry(path)).getFile());
	}

	private void executeCommand(String file, File dir) {
		try {

			MessageConsole myConsole = findConsole("Nao SDK");
			myConsole.activate();
			MessageConsoleStream out = myConsole.newMessageStream();

			out.println("Executing command: ");
			out.println("---------------------------------");

			String terminalCommand = "cmd /c ";
			if (System.getProperty("os.name").toLowerCase().contains("mac")) {
				terminalCommand = "/bin/bash -c ";
			}

			// Execute command
			Process child = Runtime.getRuntime().exec(terminalCommand + file,
					null, dir);

			copyStream(child.getInputStream(), out);
			copyStream(child.getErrorStream(), out);

			int termination = child.waitFor();
			// Get output stream to write from it

		} catch (IOException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public static void copyStream(InputStream input, OutputStream output)
			throws IOException {
		byte[] buffer = new byte[1024]; // Adjust if you want
		int bytesRead;
		while ((bytesRead = input.read(buffer)) != -1) {
			output.write(buffer, 0, bytesRead);
		}
	}

	private MessageConsole findConsole(String name) {
		ConsolePlugin plugin = ConsolePlugin.getDefault();
		IConsoleManager conMan = plugin.getConsoleManager();
		IConsole[] existing = conMan.getConsoles();
		for (int i = 0; i < existing.length; i++)
			if (name.equals(existing[i].getName()))
				return (MessageConsole) existing[i];
		// no console found, so create a new one
		MessageConsole myConsole = new MessageConsole(name, null);
		conMan.addConsoles(new IConsole[] { myConsole });
		return myConsole;
	}
}
