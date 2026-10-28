#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

//Handle keyboard interrupt
void signalHandler(int sig){
    printf("\nClinic system closed.\n");
    exit(0);
}

//Doctor child process
void doctor(char message[]){
    printf("\n[DOCTOR]\n");
    printf("Patient %s is ready for consultation.\n", message);
    printf("Consultation completed for %s.\n", message);
}

//Pharmacy child process
void pharmacy(char message[]){
    printf("\n[PHARMACY]\n");
    printf("Patient %s is ready for medication.\n", message);
    printf("Prescription processed for %s.\n", message);
}

//Billing child process
void billing(char message[]){
    printf("\n[BILLING]\n");
    printf("Billing record created for %s.\n", message);
    printf("Payment completed for %s.\n", message);
}

int main(void){
    pid_t pid;
    char message[200];
    int pipe1[2], pipe2[2], pipe3[2];

    //Set up SIGINT handler
    signal(SIGINT, signalHandler);

    printf("========================================\n");
    printf("        CLINIC MANAGEMENT SYSTEM\n");
    printf("========================================\n");

    while (1){
	//Parent process receives patient name
        printf("\n[RECEPTION]\n");
        printf("Enter patient name: ");

        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';

        printf("Patient %s registered.\n", message);
	//Create pipes for communication between processes
        if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1){
            perror("pipe");
            exit(EXIT_FAILURE);
        }
	//Create Doctor child process using fork()
        pid = fork();

        if (pid < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0){
            close(pipe1[1]);
            read(pipe1[0], message, sizeof(message));
            close(pipe1[0]);

	    //Child process displays the patient message
            doctor(message);

            close(pipe2[0]);
            write(pipe2[1], message, sizeof(message));
            close(pipe2[1]);

            exit(EXIT_SUCCESS);
        }

        close(pipe1[0]);
        write(pipe1[1], message, sizeof(message));
        close(pipe1[1]);

        wait(NULL);

	//Create Pharmacy child process using fork()
        pid = fork();

        if (pid < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0){
            close(pipe2[1]);
            read(pipe2[0], message, sizeof(message));
            close(pipe2[0]);

	    //Child process displays the patient message
            pharmacy(message);

            close(pipe3[0]);
            write(pipe3[1], message, sizeof(message));
            close(pipe3[1]);
    exit(EXIT_SUCCESS);
        }

        wait(NULL);

	//Create Billing child process using fork()
        pid = fork();

        if (pid < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0){
            close(pipe3[1]);
            read(pipe3[0], message, sizeof(message));
            close(pipe3[0]);

	    //Child process displays the patient message
            billing(message);

            exit(EXIT_SUCCESS);
        }

        wait(NULL);

        printf("\n----------------------------------------\n");
        printf("Please enter the next patient name.\n");
        printf("Press Ctrl+C to exit the system.\n");
        printf("----------------------------------------\n");
    }

    return 0;
}
