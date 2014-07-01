package naoplugin.handlers;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URISyntaxException;

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
import org.osgi.framework.Bundle;

/**
 * Our sample handler extends AbstractHandler, an IHandler base class.
 * 
 * @see org.eclipse.core.commands.IHandler
 * @see org.eclipse.core.commands.AbstractHandler
 */
public class SampleHandler extends AbstractHandler {
	/**
	 * The constructor.
	 */
	public SampleHandler() {
	}

	/**
	 * the command has been executed, so extract extract the needed information
	 * from the application context.
	 */
	public Object execute(ExecutionEvent event) throws ExecutionException {
		try {

			MessageConsole myConsole = findConsole("Nao SDK");
			myConsole.activate();
			MessageConsoleStream out = myConsole.newMessageStream();
			out.println("Compiling for Nao ...");

			executeCommand("mvn", null);

			executeCommand("start test.bat", resolveBundleFile("cmds"));

			// executeCommand("C:/Users/Fabian/text.bat");

			//
			// executeCommand(String.format("unzip %s -d %s",
			// resolveBundleFile("files/datalog-2.4.zip"),
			// stateLocation.toOSString()));
			//
			// executeCommand("ls " + stateLocation.toOSString());

			/*
			 * WizardDialog wizardDialog = new WizardDialog(window.getShell(),
			 * new MyWizard());
			 * 
			 * if (wizardDialog.open() == Window.OK) {
			 * System.out.println("Ok pressed"); } else {
			 * System.out.println("Cancel pressed"); } /*
			 * MessageDialog.openInformation(window.getShell(),
			 * "Uni Mannheim - Robot Project", "Hello, Team :)");
			 */
		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
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

			// Execute command
			Process child = Runtime.getRuntime().exec("cmd /c " + file, null,
					dir);

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
