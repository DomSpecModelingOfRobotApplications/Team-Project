package org.drm;

import org.eclipse.jface.preference.DirectoryFieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.jface.preference.FileFieldEditor;
import org.eclipse.jface.preference.StringFieldEditor;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;

public class PreferencePage extends FieldEditorPreferencePage implements
		IWorkbenchPreferencePage {

	public PreferencePage() {
		super(GRID);

	}

	public void createFieldEditors() {
		addField(new FileFieldEditor("MELANEE_OUTPUT", "Melanee Output:",
				getFieldEditorParent()));

		addField(new DirectoryFieldEditor("BUILD_TOOLS",
				"Build Tools Directory:", getFieldEditorParent()));

		addField(new DirectoryFieldEditor("EXECUTION_FOLDER",
				"NAO SDK Bin Directory:", getFieldEditorParent()));

		addField(new StringFieldEditor("ROBOT_IP_ADDRESS", "Robot IP:",
				getFieldEditorParent()));

		addField(new StringFieldEditor("ROBOT_PORT", "Robot Port:",
				getFieldEditorParent()));
	}

	@Override
	public void init(IWorkbench workbench) {
		setPreferenceStore(Activator.getDefault().getPreferenceStore());
		setDescription("Deep Robot Model Preferences");
	}
}