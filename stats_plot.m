clear;

% Import data
stats = csvread('../statistics/stats_qplayer.csv');
N_vals = length(stats);
step = 1/(N_vals-1);

%
% Plot
figure;
x_vals = 0:step:1;
plot(x_vals, stats, '*b');
title('Win rate');
xlabel('p-value');
ylabel('win rate');
%}

%{
% Plot
figure;
plot(stats, '*-b');
title('Optimal move');
xlabel('Number of sticks left');
ylabel('Optimal move fraction');
%}

