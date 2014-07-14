package eclipseplugin;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import naoplugin.Activator;

import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.Platform;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.requests.ChangePropertyValueRequest;
import org.eclipse.gmf.runtime.diagram.ui.tools.AbstractPopupBarTool;
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
import org.osgi.framework.Bundle;

public class PlayButton implements IPopupToolBarProvider {

	public PlayButton() {
		// TODO Auto-generated constructor stub
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
			// *********************************
			// * Add reasoning option
			// ********************************
			tool = new Button(editPart, null);

			imageDescriptor = Activator.imageDescriptorFromPlugin(
					Activator.PLUGIN_ID, "/icons/play.gif");
			image = imageDescriptor.createImage();

			result.add(new PopupToolBarToolWrapper(image, tool,
					"Do Auto Layout", "DRM"));
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
					"Do Auto Layout", "Do Auto Layout");
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
						.getTextFromModel((Element) part
								.resolveSemanticElement());
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
		}

		public static final String MELANEE_OUTPUT = "/Users/fabiankajzar/OutputTest/script.cpp";//"D:\\Mannheim\\TeamProject\\Team-Project\\NAO_API\\VS_project\\script.cpp";
		public static final String BUILD_TOOLS = "D:\\Mannheim\\TeamProject\\Team-Project\\NAO_API\\VS_project";
		public static final String BUILD_COMMAND = "qibuild make --release";
		public static final String EXECUTION_FOLDER = "D:\\Mannheim\\TeamProject\\Team-Project\\NAO_API\\VS_project\\build-naoqi-sdk\\sdk\\bin";
		public static final String EXECUTION_COMMAND = "apidemonstration.exe --pip 127.0.0.1 --pport 9559";

		private PrintWriter writeFile(String path) throws Exception {
			// write file
			File target = new File(path);
			PrintWriter writer = new PrintWriter(target.toString(), "UTF-8");
			return writer;
		}

		private File resolveBundleFile(String path) throws URISyntaxException,
				IOException {
			Bundle bundle = Platform.getBundle("NaoPlugin");
			return new File(FileLocator.resolve(bundle.getEntry(path))
					.getFile());
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
				Process child = Runtime.getRuntime().exec(
						terminalCommand + file, null, dir);

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

}
