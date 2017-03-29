clear;

% Import data
stats = csvread('../statistics/stats_xplayer.csv');
N_vals = length(stats);

%
% Plot
figure;
x_vals = linspace(0, 1, N_vals);
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

