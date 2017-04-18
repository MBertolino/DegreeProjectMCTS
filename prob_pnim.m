% An attempt to plot the probability of the states
% for different boards in the perturbed Nim.

clear all; close all;

p = 0:0.01:1; % Probability of perturbation
X = 10; % # of states
[Px0, Px10, Px01, ...
    Px20, Px11, Px02, ...
    Px30, Px21, Px12, ...
    Px03] = deal(zeros(length(p), X));

for i = 1:length(p)
    % Px0
    % 0|1
    % 0|0
    for j = 1:X
        Px0(i, j) = p(i).^(j-1)*(1-p(i));
    end
    
    % Px10 Px20
    % 1|11  11|111
    % 0|0    0|0
    Px10(i, 2) = (1 - p(i))*Px0(i, 1);
    Px10(i, 3) = (1 - p(i))*Px0(i, 2) + p(i)*Px0(i, 1);
    Px20(i, 3) = (1 - p(i))*Px0(i, 1);
    for j = 4:X
        Px10(i, j) = (1 - p(i))*Px0(i, j-1) + p(i)*Px0(i, j-2);
        Px20(i, j) = (1 - p(i))*Px0(i, j-2) + p(i)*Px0(i, j-3);
    end
    
    % Px30
    % 111|1111
    %   0|0
    Px30(i, 4) = (1 - p(i))*Px0(i, 1);
    for j = 5:X
        Px30(i, j) = (1 - p(i))*Px0(i, j-3) + p(i)*Px0(i, j-4);
    end
    
    % Px01 Px11 Px21
    % 0|1  1|11  11|111
    % 1|1  1|1    1|1
    Px01(i, 2) = (1 - p(i))*Px0(i, 1);
    Px01(i, 1) = (1 - p(i))*Px0(i, 2) + 0.5*p(i)*(Px10(i, 2) + Px01(i, 2));
    
    Px11(i, 1) = 0.5*(1 - p(i))*(Px10(i, 2) + Px01(i, 2));
    for j = 3:2:X-1
        Px01(i, j+1) = (1 - p(i))*Px0(i, j) + 0.5*p(i)*(Px10(i, j) + Px01(i, j-2));
        Px01(i, j) = (1 - p(i))*Px0(i, j+1) + 0.5*p(i)*(Px10(i, j+1) + Px01(i, j+1));
        Px11(i, j+1) = 0.5*(1 - p(i))*(Px10(i, j) + Px01(i, j-2)) + ...
            p(i)/3*(Px01(i, j-1) + Px20(i, j) + Px11(i, j-2));
        Px11(i, j) = 0.5*(1 - p(i))*(Px10(i, j+1) + Px01(i, j+1)) + ...
            p(i)/3*(Px01(i, j-2) + Px20(i, j+1) + Px11(i, j+1));
    end
    
    Px21(i, 4) = (1 - p(i))/3*(Px01(i, 2) + Px20(i, 3) + Px11(i, 1));
    Px21(i, 3) = (1 - p(i))/3*(Px01(i, 1) + Px20(i, 4) + Px11(i, 4)) + ...
        0.25*p(i)*(Px01(i, 2) + Px11(i, 1) + Px30(i, 4) + Px21(i, 4));
    for j = 5:2:X-1
        Px21(i, j+1) = (1 - p(i))/3*(Px01(i, j-1) + Px20(i, j) + Px11(i, j-2)) + ...
            0.25*p(i)*(Px01(i, j-4) + Px11(i, j-1) + Px30(i, j) + Px21(i, j-2));
        Px21(i, j) = (1 - p(i))/3*(Px01(i, j-2) + Px20(i, j+1) + Px11(i, j+1)) + ...
            0.25*p(i)*(Px01(i, j-1) + Px11(i, j-2) + Px30(i, j+1) + Px21(i, j+1));
    end
    
    % Px02
    %  0|1
    % 11|11
    Px02(i, 3) = (1 - p(i))*Px0(i, 1); 
    for j = 4:4:X-3
        Px02(i, j) = (1 - p(i))*Px0(i, j-2) + p(i)/3*(Px10(i, j-2) + Px11(i, j-3) + Px02(i, j-1));
        Px02(i, j-3) = (1 - p(i))*Px0(i, j-1) + p(i)/3*(Px10(i, j-1) + Px11(i, j) + Px02(i, j));
        Px02(i, j-2) = (1 - p(i))*Px0(i, j) + p(i)/3*(Px10(i, j) + Px11(i, j-1) + Px02(i, j-3));
        Px02(i, j+3) = (1 - p(i))*Px0(i, j+1) + p(i)/3*(Px10(i, j+1) + Px11(i, j+2) + Px02(i, j-2));
    end
    
    % Px12
    Px12(i, 4) = (1 - p(i))/3*(Px10(i, 2) + Px11(i, 1) + Px02(i, 3));
    Px12(i, 1) = (1 - p(i))/3*(Px10(i, 3) + Px11(i, 4) + Px02(i, 4)) + ...
        0.25*p(i)*(Px20(i, 3) + Px02(i, 3) + Px21(i, 4) + Px12(i, 4));
    Px12(i, 2) = (1 - p(i))/3*(Px10(i, 4) + Px11(i, 3) + Px02(i, 1)) + ...
        0.25*p(i)*(Px20(i, 4) + Px02(i, 4) + Px21(i, 3) + Px12(i, 1));
    for j = 5:4:X-3
        Px12(i, j+2) = (1 - p(i))/3*(Px10(i, j) + Px11(i, j+1) + Px02(i, j-3)) + ...
            0.25*p(i)*(Px20(i, j) + Px02(i, j-4) + Px21(i, j+1) + Px12(i, j-3));
        Px12(i, j+3) = (1 - p(i))/3*(Px10(i, j+1) + Px11(i, j) + Px02(i, j+2)) + ...
            0.25*p(i)*(Px20(i, j+1) + Px02(i, j-3) + Px21(i, j) + Px12(i, j+2));
        Px12(i, j) = (1 - p(i))/3*(Px10(i, j+2) + Px11(i, j+3) + Px02(i, j+3)) + ...
            0.25*p(i)*(Px20(i, j+2) + Px02(i, j+2) + Px21(i, j+3) + Px12(i, j+3));
        Px12(i, j+1) = (1 - p(i))/3*(Px10(i, j+3) + Px11(i, j+2) + Px02(i, j)) + ...
            0.25*p(i)*(Px20(i, j+3) + Px02(i, j+3) + Px21(i, j+2) + Px12(i, j));
    end
    
    % Px03
    Px03(i, 4) = (1 - p(i))*Px0(i, 1);
    Px03(i, 3) = (1 - p(i))*Px0(i, 2) + ...
        0.25*p(i)*(Px10(i, 2) + Px11(i, 1) + Px12(i, 4) + Px03(i, 4));
    Px03(i, 2) = (1 - p(i))*Px0(i, 3) + ...
        0.25*p(i)*(Px10(i, 3) + Px11(i, 4) + Px12(i, 1) + Px03(i, 3));
    Px03(i, 1) = (1 - p(i))*Px0(i, 4) + ...
        0.25*p(i)*(Px10(i, 4) + Px11(i, 3) + Px12(i, 2) + Px03(i, 2));
    for j = 5:4:X-3
    Px03(i, j+3) = (1 - p(i))*Px0(i, j) + ...
        0.25*p(i)*(Px10(i, j) + Px11(i, j+1) + Px12(i, j+2) + Px03(i, j-4));
    Px03(i, j+2) = (1 - p(i))*Px0(i, j+1) + ...
        0.25*p(i)*(Px10(i, j+1) + Px11(i, j) + Px12(i, j+3) + Px03(i, j+3));
    Px03(i, j+1) = (1 - p(i))*Px0(i, j+2) + ...
        0.25*p(i)*(Px10(i, j+2) + Px11(i, j+3) + Px12(i, j) + Px03(i, j+2));
    Px03(i, j) = (1 - p(i))*Px0(i, j+3) + ...
        0.25*p(i)*(Px10(i, j+3) + Px11(i, j+2) + Px12(i, j+1) + Px03(i, j+1));
    
    end
end


% Plot probabilities
figure()
% subplot(2, 2, 1)
% plot(p, Px0(:,1))
% title('Px0')
% xlabel('p')
% ylabel('Probability X = j')
% legend('X = 0', 'X = 1', 'X = 2', 'X = 3', 'X = 4')

% hold on;
% subplot(2, 2, 2)
% plot(p, Px10(:,1));
% title('Px10')
% xlabel('p')
% ylabel('Probability X = j')
% legend('X = 0', 'X = 1', 'X = 2', 'X = 3', 'X = 4')

% subplot(2, 2, 3)
% plot(p, Px01(:,1));
% title('Px01')
% xlabel('p')
% ylabel('Probability X = j')
% legend('X = 0', 'X = 1', 'X = 2', 'X = 3', 'X = 4')

% subplot(2, 2, 4)
% plot(p, Px20(:,1));
% title('Px20')
% xlabel('p')
% ylabel('Probability X = j')
% legend('X = 0', 'X = 1', 'X = 2', 'X = 3', 'X = 4')

% figure()
% subplot(2, 2, 1)
% plot(p, Px11(:,1));
% title('Px11')
% xlabel('p')
% ylabel('Probability X = j')
% legend('X = 0', 'X = 1', 'X = 2', 'X = 3', 'X = 4')

% subplot(2, 2, 2)
% plot(p, Px02(:,1));
% title('Px02')
% xlabel('p')
% ylabel('Probability X = j')
% legend('X = 0', 'X = 1', 'X = 2', 'X = 3', 'X = 4')

% subplot(2, 2, 3)
% plot(p, Px30(:,1));
% title('Px30')
% xlabel('p')
% ylabel('Probability X = j')
% legend('X = 3', 'X = 4', 'X = 5', 'X = 6', 'X = 7')

% subplot(2, 2, 4)
plot(p, Px21);
title('Px21')
xlabel('p')
ylabel('Probability X = j')
legend('X = 2', 'X = 3', 'X = 4', 'X = 5', 'X = 6', 'X = 7', 'X = 8', 'X = 9')

% figure()
% plot(p, Px12);
% title('Px12')
% xlabel('p')
% ylabel('Probability X = j')
% legend('X = 0', 'X = 1', 'X = 2', 'X = 3', 'X = 4', 'X = 5', 'X = 6', 'X = 7', 'X = 8', 'X = 9')

%figure()
% plot(p, Px03(:,1));
% title('Px03')
% xlabel('p')
% ylabel('Probability X = j')
% legend('X = 0', 'X = 1', 'X = 2', 'X = 3', 'X = 4', 'X = 5', 'X = 6', 'X = 7', 'X = 8', 'X = 9')

% legend('Px0', 'Px01', 'Px11', 'Px21', 'Px12', 'Px03')
%legend('Px0', 'Px10', 'Px01', 'Px20', 'Px11', 'Px02', 'Px30', 'Px21', 'Px12')