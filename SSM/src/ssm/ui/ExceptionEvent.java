package ssm.ui;
// File: ExceptionEvent.java

import java.util.EventObject;

public class ExceptionEvent extends EventObject {
	private static final long serialVersionUID = 1L;
	String ExceptionMessage;

	// source cannot be null
	public ExceptionEvent(Object source, String exceptionMessage) {
		super(source);
		ExceptionMessage = exceptionMessage;
	}

	public String getMessage() {
		return ExceptionMessage;
	}

	public String toString() {
		return ExceptionMessage;
	}
}
