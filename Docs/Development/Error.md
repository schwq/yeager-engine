### This document was created to explain about the errors and warnings that the engine can throw during compile-time and runtime

### Section 1: Panic and the critical state
During the execution of the engine, critical errors, *those which the engine cannot run anymore*, are called panic. If a panic is thrown during execution, something really bad have happen and the engine cannot ensure the safety of projects and code written inside of it, so the best solution is just to force a crash. These kind of errors can be a framework not been loaded properlly, like OpenGL or GLAD not been found in the system. The *private* folders of the engine not been found in the C:\Program Files and so on. The best way of dealing with these erros is to read the log created by it at the public folder .

