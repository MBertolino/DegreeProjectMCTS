clear;

% Import data
stats = csvread('../statistics/stats_xplayer.csv');
N_vals = length(stats);
step = 1/N_vals;

%
% Plot
figure;
x_vals = 0:step:1-step;
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

