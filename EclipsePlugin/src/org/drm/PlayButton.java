package org.drm;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.requests.ChangePropertyValueRequest;
import org.eclipse.gmf.runtime.diagram.ui.tools.AbstractPopupBarTool;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.console.ConsolePlugin;
import org.eclipse.ui.console.IConsole;
import org.eclipse.ui.console.IConsoleManager;
import org.eclipse.ui.console.MessageConsole;
import org.eclipse.ui.console.MessageConsoleStream;
import org.melanee.core.models.plm.PLM.Element;
import org.melanee.core.models.plm.PLM.Level;
import org.melanee.core.workbench.interfaces.IPopupToolBarProvider;
import org.melanee.textdsl.modeleditor.editor.sourceviewerconfiguration.MultiLevelModelColorConstants;
import org.melanee.textdsl.modeleditor.editor.sourceviewerconfiguration.MultiLevelModelPartitionScanner;
import org.melanee.textdsl.modeleditor.editor.sourceviewerconfiguration.MultilevelLiteralScanner;
import org.melanee.textdsl.modeleditor.textualdslmodelinterpreter.TextualDSLModelInterpreter;
import org.melanee.textdsl.models.weaving.M2TWeaving.M2TWeavingFactory;

public class PlayButton implements IPopupToolBarProvider {

	public PlayButton() {
	}

	@Override
	public List<PopupToolBarToolWrapper> getPopUpToolbarButtonsForDiagramElement(
			Object host) {

		IGraphicalEditPart editPart = (IGraphicalEditPart) host;

		List<PopupToolBarToolWrapper> result = new ArrayList<PopupToolBarToolWrapper>();
		AbstractPopupBarTool tool = null;
		ImageDescriptor imageDescriptor = null;
		Image image = null;

		if (editPart.resolveSemanticElement() instanceof Level) {
			tool = new Button(editPart, null);

			imageDescriptor = Activator.imageDescriptorFromPlugin(
					Activator.PLUGIN_ID, "/icons/play.gif");
			image = imageDescriptor.createImage();

			result.add(new PopupToolBarToolWrapper(image, tool,
					"Execute on Robot", "DRM"));
		}

		return result;
	}

	static class Button extends AbstractPopupBarTool {
		public Button(EditPart epHost, CreateRequest theRequest) {
			super(epHost, theRequest);
		}

		@Override
		protected Request createTargetRequest() {
			ChangePropertyValueRequest req = new ChangePropertyValueRequest(
					"Run on Robot", "Run on Robot");
			return req;
		}

		@Override
		protected Command getCommand() {
			return new PlayCommand((IGraphicalEditPart) getHost());
		}
	}

	static class PlayCommand extends Command {

		IGraphicalEditPart part;

		public PlayCommand(IGraphicalEditPart part) {
			super();
			this.part = part;
		}

		@Override
		public void execute() {
			MessageConsole myConsole = findConsole("Deep Robot Model");
			myConsole.activate();
			MessageConsoleStream out = myConsole.newMessageStream();

			try {

				// read preference
				IPreferenceStore store = Activator.getDefault()
						.getPreferenceStore();

				// "D:\\Mannheim\\TeamProject\\Team-Project\\NAO_API\\VS_project\\script.cpp");
				String MELANEE_OUTPUT = store.getString("MELANEE_OUTPUT");

				// "D:\\Mannheim\\TeamProject\\Team-Project\\NAO_API\\VS_project");
				String BUILD_TOOLS = store.getString("BUILD_TOOLS");

				String BUILD_COMMAND = "qibuild make --release";

				// "D:\\Mannheim\\TeamProject\\Team-Project\\NAO_API\\VS_project\\build-naoqi-sdk\\sdk\\bin");
				String EXECUTION_FOLDER = store.getString("EXECUTION_FOLDER");

				// "127.0.0.1"
				String ROBOT_IP_ADDRESS = store.getString("ROBOT_IP_ADDRESS");

				// "9559"
				String ROBOT_PORT = store.getString("ROBOT_PORT");

				String EXECUTION_COMMAND = String.format(
						"apidemonstration.exe --pip %s --pport %s",
						ROBOT_IP_ADDRESS, ROBOT_PORT);

				out.println(MELANEE_OUTPUT);

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
						.getTextFromModel((Element) part
								.resolveSemanticElement());
				writer.println(textFromModel);

				// template post
				writer.println("}");

				writer.flush();
				writer.close();

				// invoke build
				executeCommand(BUILD_COMMAND, new File(BUILD_TOOLS), true);

				// execute run on NAO
				executeCommand(EXECUTION_COMMAND, new File(EXECUTION_FOLDER),
						false);
			} catch (Exception e) {
				e.printStackTrace();
				out.println(e.toString());
			}
		}

		private PrintWriter writeFile(String path) throws Exception {
			// write file
			File target = new File(path);
			PrintWriter writer = new PrintWriter(target.toString(), "UTF-8");
			return writer;
		}

		/**
		 * executes the command
		 * 
		 * @param command
		 * @param dir
		 * @param waitFor
		 */
		private void executeCommand(String command, File dir, boolean waitFor) {
			try {

				MessageConsole myConsole = findConsole("Nao SDK");
				myConsole.activate();
				MessageConsoleStream out = myConsole.newMessageStream();

				out.println("Executing command: " + command);
				out.println("---------------------------------");

				String terminalCommand = "cmd /c ";
				if (System.getProperty("os.name").toLowerCase().contains("mac")) {
					terminalCommand = "/bin/bash -c ";
				}

				// Execute command
				Process child = Runtime.getRuntime().exec(
						terminalCommand + command, null, dir);

				copyStream(child.getInputStream(), out);
				copyStream(child.getErrorStream(), out);

				if (waitFor)
					child.waitFor();
				// Get output stream to write from it

			} catch (IOException e) {
				e.printStackTrace();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

		/**
		 * copies the input to the output stream
		 * 
		 * @param input
		 * @param output
		 * @throws IOException
		 */
		public static void copyStream(InputStream input, OutputStream output)
				throws IOException {
			byte[] buffer = new byte[1024]; // Adjust if you want
			int bytesRead;
			while ((bytesRead = input.read(buffer)) != -1) {
				output.write(buffer, 0, bytesRead);
			}
		}

		/**
		 * returns the console identified by name
		 * 
		 * @param name
		 * @return
		 */
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

}
