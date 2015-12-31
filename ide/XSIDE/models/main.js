function AppMain()
{
	app.trace("Enter AppMain()");
	
	var dock = app.loadDock("maindock");
	
	var form = app.loadForm("main");
	app.showForm(form);
	
}
