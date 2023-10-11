
import java.util.concurrent.Semaphore;

public class TrafficController {
    private Semaphore semaphore = new Semaphore(1);

    public void enterLeft() {
        try {
	    semaphore.acquire();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
    public void enterRight() {
        try {
	    semaphore.acquire();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }     
    }
    public void leaveLeft() {
        semaphore.release();
    }
    public void leaveRight() {
        semaphore.release();
    }

}