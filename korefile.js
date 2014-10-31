var solution = new Solution('Exercise3');
var project = new Project('Exercise3');

project.addFile('Sources/**');
project.setDebugDir('Deployment');

project.addSubProject(Solution.createProject('Kore'));

solution.addProject(project)

return solution;
