import java.util.concurrent.Semaphore;

public class TrafficController {

    private int carsOnBridge = 0;
    private Semaphore leftSemaphore = new Semaphore(1);  // Semaphore para carros da esquerda
    private Semaphore rightSemaphore = new Semaphore(1); // Semaphore para carros da direita

    public void enterLeft() {
        try {
            leftSemaphore.acquire(); // Adquire o semaphore da esquerda
            while (carsOnBridge > 0) {
                // Aguarda até que a ponte esteja vazia
                Thread.sleep(100); // Pode adicionar um tempo de espera aqui para evitar um loop ocupado
            }
            carsOnBridge++;
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    public void enterRight() {
        try {
            rightSemaphore.acquire(); // Adquire o semaphore da direita
            while (carsOnBridge > 0) {
                // Aguarda até que a ponte esteja vazia
                Thread.sleep(100); // Pode adicionar um tempo de espera aqui para evitar um loop ocupado
            }
            carsOnBridge++;
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    public void leaveLeft() {
        carsOnBridge--;
        leftSemaphore.release(); // Libera o semaphore da esquerda
    }

    public void leaveRight() {
        carsOnBridge--;
        rightSemaphore.release(); // Libera o semaphore da direita
    }
}
